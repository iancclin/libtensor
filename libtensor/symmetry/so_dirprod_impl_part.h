#ifndef LIBTENSOR_SO_DIRPROD_IMPL_PART_H
#define LIBTENSOR_SO_DIRPROD_IMPL_PART_H

#include <map>
#include "../defs.h"
#include "../exception.h"
#include "../not_implemented.h"
#include "../core/permutation_builder.h"
#include "symmetry_element_set_adapter.h"
#include "symmetry_operation_impl_base.h"
#include "partition_set.h"
#include "so_dirprod.h"
#include "se_part.h"

namespace libtensor {


/**	\brief Implementation of so_dirprod<N, M, T> for se_part<N, T>
	\tparam N Tensor order.
	\tparam T Tensor element type.

	\ingroup libtensor_symmetry
 **/
template<size_t N, size_t M, typename T>
class symmetry_operation_impl< so_dirprod<N, M, T>, se_part<N + M, T> > :
public symmetry_operation_impl_base< so_dirprod<N, M, T>, se_part<N + M, T> > {

public:
    static const char *k_clazz; //!< Class name

public:
    typedef so_dirprod<N, M, T> operation_t;
    typedef se_part<N + M, T> element_t;
    typedef symmetry_operation_params<operation_t> symmetry_operation_params_t;

protected:
    virtual void do_perform(symmetry_operation_params_t &params) const;

};


template<size_t N, size_t M, typename T>
const char *symmetry_operation_impl< so_dirprod<N, M, T>,
se_part<N + M, T> >::k_clazz =
        "symmetry_operation_impl< so_dirprod<N, M, T>, se_part<N, T> >";


template<size_t N, size_t M, typename T>
void symmetry_operation_impl< so_dirprod<N, M, T>,
se_part<N + M, T> >::do_perform(symmetry_operation_params_t &params) const {

    static const char *method = "do_perform(symmetry_operation_params_t&)";

    // Adapter type for the input groups
    typedef symmetry_element_set_adapter< N, T, se_part<N, T> > adapter1_t;
    typedef symmetry_element_set_adapter< M, T, se_part<M, T> > adapter2_t;

#ifdef LIBTENSOR_DEBUG
    //    consistency< se_part<N, T> >(params.g1).check();
    //    consistency< se_part<M, T> >(params.g2).check();
#endif

    adapter1_t g1(params.g1);
    adapter2_t g2(params.g2);
    params.g3.clear();

    // Create map at which position which index ends up.
    sequence<N + M, size_t> map(0);
    for (size_t i = 0; i < N + M; i++) map[i] = i;
    permutation<N + M> pinv(params.perm, true);
    pinv.apply(map);

    mask<N + M> tm;
    for(typename adapter1_t::iterator it1 = g1.begin();
            it1 != g1.end(); it1++) {

        const se_part<N, T> &e1 = g1.get_elem(it1);
        const mask<N> &m1 = e1.get_mask();

        mask<N + M> m3;
        for (size_t i = 0; i < N; i++) {
            if (tm[map[i]] && m1[i]) {
                throw bad_symmetry(g_ns, k_clazz, method,
                        __FILE__, __LINE__, "Overlapping masks in g1.");
            }
            tm[map[i]] |= m1[i];
            m3[map[i]] = m1[i];
        }

        se_part<N + M, T> e3(params.bis, m3, e1.get_npart());

        abs_index<N> ai(e1.get_pdims());
        do {
            const index<N> &i1a = ai.get_index();

            index<N + M> i3a;
            for (size_t i = 0; i < N; i++) i3a[map[i]] = i1a[i];

            if (e1.is_forbidden(i1a)) {
                e3.mark_forbidden(i3a);
                continue;
            }

            index<N> i1b = e1.get_direct_map(i1a);
            if (i1a == i1b) continue;

            index<N + M> i3b;
            for (size_t i = 0; i < N; i++) i3b[map[i]] = i1b[i];

            e3.add_map(i3a, i3b, e1.get_sign(i1a, i1b));

        } while (ai.inc());

        params.g3.insert(e3);
    }

    for(typename adapter2_t::iterator it2 = g2.begin();
            it2 != g2.end(); it2++) {

        const se_part<M, T> &e2 = g2.get_elem(it2);
        const mask<M> &m2 = e2.get_mask();

        mask<N + M> m3;
        for (size_t i = 0; i < M; i++) {
            if (tm[map[i + N]] && m2[i]) {
                throw bad_symmetry(g_ns, k_clazz, method,
                        __FILE__, __LINE__, "Overlapping masks in g2.");
            }
            tm[map[i + N]] |= m2[i];
            m3[map[i + N]] = m2[i];
        }

        se_part<N + M, T> e3(params.bis, m3, e2.get_npart());

        abs_index<M> ai(e2.get_pdims());
        do {
            const index<M> &i2a = ai.get_index();

            index<N + M> i3a;
            for (size_t i = 0; i < M; i++) i3a[map[i + N]] = i2a[i];

            if (e2.is_forbidden(i2a)) {
                e3.mark_forbidden(i3a);
                continue;
            }

            index<M> i2b = e2.get_direct_map(i2a);
            if (i2a == i2b) continue;

            index<N + M> i3b;
            for (size_t i = 0; i < M; i++) i3b[map[i + N]] = i2b[i];

            e3.add_map(i3a, i3b, e2.get_sign(i2a, i2b));

        } while (ai.inc());

        params.g3.insert(e3);
    }
}


} // namespace libtensor

#endif // LIBTENSOR_SO_DIRPROD_IMPL_PART_H
