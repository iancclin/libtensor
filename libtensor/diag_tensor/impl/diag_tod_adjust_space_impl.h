#ifndef LIBTENSOR_DIAG_TOD_ADJUST_SPACE_IMPL_H
#define LIBTENSOR_DIAG_TOD_ADJUST_SPACE_IMPL_H

#include <map>
#include <set>
#include <vector>
#include <libtensor/diag_tensor/diag_tensor_ctrl.h>
#include <libtensor/tod/kernels/loop_list_runner.h>
#include <libtensor/tod/kernels/kern_add_generic.h>
#include <libtensor/tod/kernels/kern_copy_generic.h>
#include <libtensor/tod/bad_dimensions.h>
#include "../diag_tod_adjust_space.h"

namespace libtensor {


template<size_t N>
const char *diag_tod_adjust_space<N>::k_clazz = "diag_tod_adjust_space<N>";


template<size_t N>
void diag_tod_adjust_space<N>::perform(diag_tensor_wr_i<N, double> &ta) {

    static const char *method = "perform(diag_tensor_wr_i<N, double>&)";

    if(!ta.get_dims().equals(m_spc.get_dims())) {
        throw bad_dimensions(g_ns, k_clazz, method, __FILE__, __LINE__, "ta");
    }

    diag_tod_adjust_space<N>::start_timer();

    try {

        const diag_tensor_space<N> &dts1 = ta.get_space();
        const diag_tensor_space<N> &dts2 = m_spc;

        diag_tensor_wr_ctrl<N, double> ca(ta);

        std::vector<size_t> ssl1, ssl2;
        dts1.get_all_subspaces(ssl1);
        dts2.get_all_subspaces(ssl2);

        //
        //  Step 1: Build mapping between old and new spaces.
        //          Expand the space of the tensor and fill with zeros.
        //

        std::map<size_t, size_t> map12, map21;
        std::set<size_t> out, remain;
        for(size_t ssi1 = 0; ssi1 < ssl1.size(); ssi1++) {

            size_t ssn1 = ssl1[ssi1];
            for(size_t ssi2 = 0; ssi2 < ssl2.size(); ssi2++) {

                size_t ssn2 = ssl2[ssi2];
                const diag_tensor_subspace<N> &ss1 = dts1.get_subspace(ssn1);
                const diag_tensor_subspace<N> &ss2 = dts2.get_subspace(ssn2);
                if(!ss1.equals(ss2)) continue;

                map12[ssn1] = ssn2;
                if(map21.find(ssn2) == map21.end()) {
                    remain.insert(ssn1);
                    map21[ssn2] = ssn1;
                } else {
                    out.insert(ssn1);
                }
            }
            if(map12.find(ssn1) == map12.end()) out.insert(ssn1);
        }

        for(size_t ssi2 = 0; ssi2 < ssl2.size(); ssi2++) {

            size_t ssn2 = ssl2[ssi2];
            if(map21.find(ssn2) != map21.end()) continue;
            const diag_tensor_subspace<N> &ss2 = dts2.get_subspace(ssn2);

            size_t ssn1n = ca.req_add_subspace(ss2);
            remain.insert(ssn1n);
            double *pa = ca.req_dataptr(ssn1n);
            diag_tod_adjust_space<N>::start_timer("zero");
            size_t sz = dts1.get_subspace_size(ssn1n);
            memset(pa, 0, sizeof(double) * sz);
            diag_tod_adjust_space<N>::stop_timer("zero");
            ca.ret_dataptr(ssn1n, pa); pa = 0;
            map12[ssn1n] = ssn2;
            map21[ssn2] = ssn1n;
        }

        //
        //  Step 2: Redistribute data
        //

        for(std::set<size_t>::const_iterator iout = out.begin();
            iout != out.end(); ++iout) {

            size_t ssn1 = *iout;
            for(std::set<size_t>::const_iterator irem = remain.begin();
                irem != remain.end(); ++irem) {

                size_t ssn2 = *irem;

                double *p1 = ca.req_dataptr(ssn1);
                double *p2 = ca.req_dataptr(ssn2);
                constrained_copy(dts1.get_dims(), dts1.get_subspace(ssn1), p1,
                    dts1.get_subspace_size(ssn1), dts1.get_subspace(ssn2), p2,
                    dts1.get_subspace_size(ssn2));
                ca.ret_dataptr(ssn1, p1); p1 = 0;
                ca.ret_dataptr(ssn2, p2); p2 = 0;
            }
        }

        //
        //  Step 3: Remove subspaces no longer in use
        //

        for(std::set<size_t>::const_iterator iout = out.begin();
            iout != out.end(); ++iout) {

            ca.req_remove_subspace(*iout);
        }

    } catch(...) {
        diag_tod_adjust_space<N>::stop_timer();
        throw;
    } 

    diag_tod_adjust_space<N>::stop_timer();
}


template<size_t N>
void diag_tod_adjust_space<N>::constrained_copy(const dimensions<N> &dims,
    const diag_tensor_subspace<N> &ss1, double *p1, size_t sz1,
    const diag_tensor_subspace<N> &ss2, double *p2, size_t sz2) {

    std::list< loop_list_node<2, 1> > lpadd1, lpadd2;
    std::list< loop_list_node<1, 1> > lpset1, lpset2;
    typename std::list< loop_list_node<2, 1> >::iterator iadd = lpadd1.end();
    typename std::list< loop_list_node<1, 1> >::iterator iset = lpset1.end();

    double zero = 0.0;

    mask<N> mdone;
    while(true) {

        size_t i = 0;
        while(i < N && mdone[i]) i++;
        if(i == N) break;

        mask<N> m0, m1, m2;
        m0[i] = true;
        do {
            mark_diags(m0, ss1, m1);
            mark_diags(m0, ss2, m2);
            m0 |= m1;
            m0 |= m2;
        } while(!m1.equals(m2));

        iadd = lpadd1.insert(lpadd1.end(), loop_list_node<2, 1>(dims[i]));
        iset = lpset1.insert(lpset1.end(), loop_list_node<1, 1>(dims[i]));
        size_t inc1 = get_increment(dims, ss1, m0);
        size_t inc2 = get_increment(dims, ss2, m0);
        iadd->stepa(0) = inc1;
        iadd->stepa(1) = 0;
        iadd->stepb(0) = inc2;
        iset->stepa(0) = 0;
        iset->stepb(0) = inc1;

        loop_registers<2, 1> radd;
        radd.m_ptra[0] = p1;
        radd.m_ptra[1] = &zero;
        radd.m_ptrb[0] = p2;
        radd.m_ptra_end[0] = p1 + sz1;
        radd.m_ptra_end[1] = &zero + 1;
        radd.m_ptrb_end[0] = p2 + sz2;

        loop_registers<1, 1> rset;
        rset.m_ptra[0] = &zero;
        rset.m_ptrb[0] = p1;
        rset.m_ptra_end[0] = &zero + 1;
        rset.m_ptrb_end[0] = p1 + sz1;

        {
            diag_tod_adjust_space<N>::start_timer("copy");
            std::auto_ptr< kernel_base<2, 1> > kern_add(
                kern_add_generic::match(1.0, 1.0, 1.0, lpadd1, lpadd2));
            loop_list_runner<2, 1>(lpadd1).run(radd, *kern_add);
            std::auto_ptr< kernel_base<1, 1> > kern_set(
                kern_copy_generic::match(1.0, lpset1, lpset2));
            loop_list_runner<1, 1>(lpset1).run(rset, *kern_set);
            diag_tod_adjust_space<N>::stop_timer("copy");
        }

        mdone |= m0;
    }
}


template<size_t N>
void diag_tod_adjust_space<N>::mark_diags(const mask<N> &m0,
    const diag_tensor_subspace<N> &ss, mask<N> &m1) {

    //  Input: one or more bits set in m0
    //  Output: for each bit set in m0, the respective diagonal is marked in m1

    size_t ndiag = ss.get_ndiag();
    for(size_t i = 0; i < ndiag; i++) {
        const mask<N> &m = ss.get_diag_mask(i);
        for(size_t j = 0; j < N; j++) if(m0[j] && m[j]) {
            m1 |= m;
            break;
        }
    }
    m1 |= m0;
}


template<size_t N>
size_t diag_tod_adjust_space<N>::get_increment(const dimensions<N> &dims,
    const diag_tensor_subspace<N> &ss, const mask<N> &m) const {

    //  Build new dimensions in which only the primary index
    //  of each diagonal exists

    index<N> i1, i2;
    mask<N> mm;

    size_t ndiag = ss.get_ndiag(); // Total number of diagonals
    const mask<N> &totm = ss.get_total_mask();
    for(size_t i = 0; i < ndiag; i++) {
        const mask<N> &dm = ss.get_diag_mask(i);
        for(size_t j = 0; j < N; j++) if(dm[j]) {
            i2[j] = dims[j] - 1;
            if(m[j]) mm[j] = true;
            break;
        }
    }
    for(size_t j = 0; j < N; j++) if(!totm[j]) {
        i2[j] = dims[j] - 1;
        mm[j] = m[j];
    }

    dimensions<N> dims2(index_range<N>(i1, i2));

    //  Now compute and return increment

    size_t inc = 0;
    for(size_t j = 0; j < N; j++) if(mm[j]) inc += dims2.get_increment(j);
    return inc;
}


} // namespace libtensor

#endif // LIBTENSOR_DIAG_TOD_ADJUST_SPACE_IMPL_H

