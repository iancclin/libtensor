#include <libtensor/core/scalar_transf_double.h>
#include <libtensor/symmetry/so_apply_se_part.h>
#include "../compare_ref.h"
#include "so_apply_se_part_test.h"

namespace libtensor {


void so_apply_se_part_test::perform() throw(libtest::test_exception) {

    test_1(false,  true, false);
    test_1(false, false, false);
    test_1(false, false,  true);
    test_1( true,  true, false);
    test_1( true, false, false);
    test_1( true, false,  true);
    test_2(false,  true, false);
    test_2(false, false, false);
    test_2(false, false,  true);
    test_2( true,  true, false);
    test_2( true, false, false);
    test_2( true, false,  true);
    test_3(false,  true, false);
    test_3(false, false, false);
    test_3(false, false,  true);
    test_3( true,  true, false);
    test_3( true, false, false);
    test_3( true, false,  true);
}


/** \test Tests that application on an empty set yields an empty set
 **/
void so_apply_se_part_test::test_1(bool keep_zero,
        bool is_asym, bool sign) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss <<  "so_apply_se_part_test::test_1(" << keep_zero << ", "
            << is_asym << ", " << sign << ")";

    typedef se_part<2, double> se_t;
    typedef so_apply<2, double> so_t;
    typedef symmetry_operation_params<so_t> params_t;
    typedef symmetry_operation_impl<so_t, se_t> so_se_part_t;

    try {

    symmetry_element_set<2, double> set1(se_t::k_sym_type);
    symmetry_element_set<2, double> set2(se_t::k_sym_type);

    permutation<2> p0;
    scalar_transf<double> tr0, tr1(-1.);
    params_t params(set1, p0, (is_asym ? tr0 : tr1),
            (sign ? tr0 : tr1), keep_zero, set2);

    so_se_part_t op;

    op.perform(params);
    if(!set2.is_empty()) {
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
            "!set2.is_empty() (1).");
    }

    index<2> i1, i2;
    i2[0] = 5; i2[1] = 5;
    block_index_space<2> bis(dimensions<2>(index_range<2>(i1, i2)));
    mask<2> m11;
    m11[0] = true; m11[1] = true;
    bis.split(m11, 3);

    index<2> i00, i11;
    i11[0] = 1; i11[1] = 1;
    se_t elem(bis, m11, 2);
    elem.add_map(i00, i11, tr0);
    set2.insert(elem);

    op.perform(params);
    if(!set2.is_empty())
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "!set2.is_empty() (2).");

    } catch(exception &e) {
        fail_test(tnss.str().c_str(), __FILE__, __LINE__, e.what());
    }
}


/** \test Tests application on a non-empty set.
 **/
void so_apply_se_part_test::test_2(bool keep_zero,
        bool is_asym, bool sign) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss <<  "so_apply_se_part_test::test_2(" << keep_zero << ", "
            << is_asym << ", " << sign << ")";

    typedef se_part<2, double> se_t;
    typedef so_apply<2, double> so_t;
    typedef symmetry_operation_params<so_t> params_t;
    typedef symmetry_operation_impl<so_t, se_t> so_se_part_t;

    try {

    index<2> i1, i2;
    i2[0] = 5; i2[1] = 5;
    block_index_space<2> bis(dimensions<2>(index_range<2>(i1, i2)));
    mask<2> m11;
    m11[0] = true; m11[1] = true;
    bis.split(m11, 3);

    index<2> i00, i11, i01, i10;
    i10[0] = 1; i01[1] = 1;
    i11[0] = 1; i11[1] = 1;
    scalar_transf<double> tr0, tr1(-1.);

    se_t elem1(bis, m11, 2);
    elem1.add_map(i00, i11, tr0);
    elem1.add_map(i01, i10, tr1);

    symmetry_element_set<2, double> set1(se_t::k_sym_type);
    symmetry_element_set<2, double> set2(se_t::k_sym_type);

    set1.insert(elem1);

    permutation<2> p0;
    params_t params(set1, p0,
            is_asym ? tr0 : tr1, sign ? tr0 : tr1, keep_zero, set2);

    so_se_part_t op;
    op.perform(params);

    if(set2.is_empty())
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "set2.is_empty().");

    symmetry_element_set_adapter<2, double, se_t> ad2(set2);
    symmetry_element_set_adapter<2, double, se_t>::iterator it = ad2.begin();

    const se_t &elem2 = ad2.get_elem(it);
    it++;
    if(it != ad2.end())
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "Expected only one element.");

    if (! elem2.map_exists(i00, i11))
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "Map [0,0]->[1,1] does not exist.");

    if (elem2.get_transf(i00, i11) != tr0)
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "Wrong transformation for [0,0]->[1,1].");

    if (is_asym) {
        if (elem2.map_exists(i01, i10))
            fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                    "Map [0,1]->[1,0] does exist.");
    }
    else {
        if (! elem2.map_exists(i01, i10))
            fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                    "Map [0,1]->[1,0] does not exist.");

        if (sign) {
            if (elem2.get_transf(i01, i10) != tr0)
                fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                        "Wrong transformaiton for [0,0]->[1,1].");
        }
        else {
            if (elem2.get_transf(i01, i10) != tr1)
                fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                        "Wrong transformation for [0,0]->[1,1].");
        }
    }

    } catch(exception &e) {
        fail_test(tnss.str().c_str(), __FILE__, __LINE__, e.what());
    }
}


/** \test Tests application on non-empty set with permutation
 **/
void so_apply_se_part_test::test_3(bool keep_zero,
        bool is_asym, bool sign) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss <<  "so_apply_se_part_test::test_3(" << keep_zero << ", "
            << is_asym << ", " << sign << ")";

    typedef se_part<2, double> se_t;
    typedef so_apply<2, double> so_t;
    typedef symmetry_operation_params<so_t> params_t;
    typedef symmetry_operation_impl<so_t, se_t> so_se_part_t;

    try {

    index<2> i1, i2;
    i2[0] = 5; i2[1] = 5;
    block_index_space<2> bis(dimensions<2>(index_range<2>(i1, i2)));
    mask<2> m11;
    m11[0] = true; m11[1] = true;
    bis.split(m11, 3);

    index<2> i00, i01, i10, i11;
    i10[0] = 1; i01[1] = 1;
    i11[0] = 1; i11[1] = 1;
    scalar_transf<double> tr0, tr1(-1.);

    se_t elem1(bis, m11, 2);
    elem1.add_map(i00, i01, tr0);
    elem1.add_map(i10, i11, tr1);

    symmetry_element_set<2, double> set1(se_t::k_sym_type);
    symmetry_element_set<2, double> set2(se_t::k_sym_type);

    set1.insert(elem1);

    permutation<2> p1; p1.permute(0, 1);
    params_t params(set1, p1,
            is_asym ? tr0 : tr1, sign ? tr0 : tr1, keep_zero, set2);

    so_se_part_t op;
    op.perform(params);

    if (set2.is_empty())
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "set2.is_empty().");

    symmetry_element_set_adapter<2, double, se_t> ad2(set2);
    symmetry_element_set_adapter<2, double, se_t>::iterator it = ad2.begin();

    const se_t &elem2 = ad2.get_elem(it);
    it++;
    if(it != ad2.end())
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "Expected only one element.");

    if (! elem2.map_exists(i00, i10))
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "Map [0,0]->[1,0] does not exist.");

    if (elem2.get_transf(i00, i10) != tr0)
        fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                "Wrong transformation for [0,0]->[1,0].");

    if (is_asym) {
        if (elem2.map_exists(i01, i11))
            fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                    "Map [0,1]->[1,1] does exist.");
    }
    else {
        if (! elem2.map_exists(i01, i11))
            fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                    "Map [0,1]->[1,1] does not exist.");

        if (sign) {
            if (elem2.get_transf(i01, i11) != tr0)
                fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                        "Wrong transformation for [0,0]->[1,0].");
        }
        else {
            if (elem2.get_transf(i01, i11) != tr1)
                fail_test(tnss.str().c_str(), __FILE__, __LINE__,
                        "Wrong transformation for [0,0]->[1,0].");
        }
    }

    } catch(exception &e) {
        fail_test(tnss.str().c_str(), __FILE__, __LINE__, e.what());
    }
}

} // namespace libtensor
