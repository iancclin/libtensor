#include <libtensor/symmetry/product_table_container.h>
#include <libtensor/symmetry/so_merge_se_label.h>
#include "so_merge_se_label_test.h"

namespace libtensor {

void so_merge_se_label_test::perform() throw(libtest::test_exception) {

    std::string table_id = "S6";
    setup_pg_table(table_id);

    try {

        test_empty_1(table_id);
        test_empty_2(table_id);
        test_nm1_1(table_id);
        test_nm1_2(table_id);
        test_2n2nn_1(table_id);
        test_2n2nn_2(table_id, true);
        test_2n2nn_2(table_id, false);
        test_nmk_1(table_id, true);
        test_nmk_1(table_id, false);
        test_nmk_2(table_id, true);
        test_nmk_2(table_id, false);

    } catch (libtest::test_exception &e) {
        clear_pg_table(table_id);
        throw;
    }

    clear_pg_table(table_id);
}


/** \test Tests that a single merge of 2 dim of an empty group yields an
        empty group of lower order
 **/
void so_merge_se_label_test::test_empty_1(
        const std::string &table_id) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_empty_1(" << table_id << ")";
    std::string tns(tnss.str());

    typedef se_label<4, double> se4_t;
    typedef se_label<3, double> se3_t;
    typedef so_merge<4, 1, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se3_t> so_merge_se_t;

    symmetry_element_set<4, double> set1(se4_t::k_sym_type);
    symmetry_element_set<3, double> set2(se3_t::k_sym_type);

    mask<4> msk; msk[2] = true; msk[3] = true;
    sequence<4, size_t> seq(0);
    symmetry_operation_params<so_merge_t> params(set1, msk, seq, set2);

    so_merge_se_t().perform(params);

    if(!set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__, "Expected an empty set.");
    }
}

/** \test Tests that a double merge of dimensions of an empty group
        yields an empty group of lower order
 **/
void so_merge_se_label_test::test_empty_2(
        const std::string &table_id) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_empty_2(" << table_id << ")";
    std::string tns(tnss.str());

    typedef se_label<5, double> se5_t;
    typedef se_label<3, double> se3_t;
    typedef so_merge<5, 2, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se3_t> so_merge_se_t;

    symmetry_element_set<5, double> set1(se5_t::k_sym_type);
    symmetry_element_set<3, double> set2(se3_t::k_sym_type);

    mask<5> msk; msk[0] = msk[1] = msk[2] = msk[3] = true;
    sequence<5, size_t> seq(0); seq[2] = seq[3] = 1;
    symmetry_operation_params<so_merge_t> params(set1, msk, seq, set2);

    so_merge_se_t().perform(params);

    if(!set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__, "Expected an empty set.");
    }

}

/** \test Merge of 2 dim of a 3-space on a 1-space in one step.
 **/
void so_merge_se_label_test::test_nm1_1(
        const std::string &table_id) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_nm1_1(" << table_id << ")";
    std::string tns(tnss.str());

    typedef se_label<2, double> se2_t;
    typedef se_label<3, double> se3_t;
    typedef so_merge<3, 1, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se2_t> so_merge_se_t;

    index<3> i1a, i1b;
    i1b[0] = 3; i1b[1] = 3; i1b[2] = 3;
    dimensions<3> bidims1(index_range<3>(i1a, i1b));
    se3_t el1(bidims1, table_id);
    {
        block_labeling<3> &bl1 = el1.get_labeling();
        mask<3> m1; m1[0] = m1[1] = m1[2] = true;
        for (size_t i = 0; i < 4; i++) bl1.assign(m1, i, i);
        el1.set_rule(2);
    }

    symmetry_element_set<3, double> set1(se3_t::k_sym_type);
    symmetry_element_set<2, double> set2(se2_t::k_sym_type);

    set1.insert(el1);
    mask<3> m; m[1] = m[2] = true;
    sequence<3, size_t> seq(0);
    symmetry_operation_params<so_merge_t> params(set1, m, seq, set2);

    so_merge_se_t().perform(params);

    if(set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected a non-empty set.");
    }

    symmetry_element_set_adapter<2, double, se2_t> adapter(set2);
    symmetry_element_set_adapter<2, double, se2_t>::iterator it =
            adapter.begin();
    const se2_t &el2 = adapter.get_elem(it);
    it++;
    if(it != adapter.end()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected only one element.");
    }

    const dimensions<2> &bidims2 = el2.get_labeling().get_block_index_dims();
    std::vector<bool> rx(bidims2.get_size(), false);

    index<2> idx;
    size_t ii = 0, ij = 1;
    idx[ii] = 2;
    for (size_t j = 0; j < 4; j++) {
        idx[ij] = j;
        rx[abs_index<2>(idx, bidims2).get_abs_index()] = true;
    }
    idx[ii] = 3; idx[ij] = 1;
    rx[abs_index<2>(idx, bidims2).get_abs_index()] = true;
    idx[ij] = 3;
    rx[abs_index<2>(idx, bidims2).get_abs_index()] = true;

    check_allowed(tns.c_str(), "el2", el2, rx);
}


/** \test Single merge of 3 dim of a 3-space on a 1-space.
 **/
void so_merge_se_label_test::test_nm1_2(
        const std::string &table_id) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_nm1_2(" << table_id << ")";
    std::string tns(tnss.str());

    typedef se_label<3, double> se3_t;
    typedef se_label<1, double> se1_t;
    typedef so_merge<3, 2, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se1_t> so_merge_se_t;

    index<3> i1a, i1b;
    i1b[0] = 3; i1b[1] = 3; i1b[2] = 3;
    dimensions<3> bidims1(index_range<3>(i1a, i1b));
    mask<3> m; m[0] = m[1] = m[2] = true;

    se3_t el1(bidims1, table_id);
    {
        block_labeling<3> &bl1 = el1.get_labeling();
        for (size_t i = 0; i < 4; i++) bl1.assign(m, i, i);
        el1.set_rule(2);
    }

    symmetry_element_set<3, double> set1(se3_t::k_sym_type);
    symmetry_element_set<1, double> set2(se1_t::k_sym_type);

    set1.insert(el1);
    sequence<3, size_t> seq(0);
    symmetry_operation_params<so_merge_t> params(set1, m, seq, set2);

    so_merge_se_t().perform(params);

    if(set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected a non-empty set.");
    }

    symmetry_element_set_adapter<1, double, se1_t> adapter(set2);
    symmetry_element_set_adapter<1, double, se1_t>::iterator it =
            adapter.begin();
    const se1_t &el2 = adapter.get_elem(it);
    it++;
    if(it != adapter.end()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected only one element.");
    }

    const dimensions<1> &bidims2 = el2.get_labeling().get_block_index_dims();
    std::vector<bool> rx(bidims2.get_size(), false);

    rx[2] = rx[3] = true;
    check_allowed(tns.c_str(), "el2", el2, rx);
}

/** \test Double merge of 4 dim of a 4-space on a 2-space (simple rule)
 **/
void so_merge_se_label_test::test_2n2nn_1(
        const std::string &table_id) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_2n2nn_1(" << table_id << ")";
    std::string tns(tnss.str());

    typedef se_label<4, double> se4_t;
    typedef se_label<2, double> se2_t;
    typedef so_merge<4, 2, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se2_t> so_merge_se_t;

    index<4> i1a, i1b;
    i1b[0] = 3; i1b[1] = 3; i1b[2] = 3; i1b[3] = 3;
    dimensions<4> bidims1(index_range<4>(i1a, i1b));

    se4_t el1(bidims1, table_id);
    {
        block_labeling<4> &bl1 = el1.get_labeling();
        mask<4> m1; m1[0] = m1[1] = m1[2] = m1[3] = true;
        for (size_t i = 0; i < 4; i++) bl1.assign(m1, i, i);
        el1.set_rule(0);
    }

    symmetry_element_set<4, double> set1(se4_t::k_sym_type);
    symmetry_element_set<2, double> set2(se2_t::k_sym_type);

    set1.insert(el1);
    mask<4> m; m[0] = m[2] = m[1] = m[3] = true;
    sequence<4, size_t> seq(0); seq[1] = seq[3] = 1;
    symmetry_operation_params<so_merge_t> params(set1, m, seq, set2);

    so_merge_se_t().perform(params);

    if(set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected a non-empty set.");
    }

    symmetry_element_set_adapter<2, double, se2_t> adapter(set2);
    symmetry_element_set_adapter<2, double, se2_t>::iterator it =
            adapter.begin();
    const se2_t &el2 = adapter.get_elem(it);
    it++;
    if(it != adapter.end()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected only one element.");
    }

    const dimensions<2> &bidims2 = el2.get_labeling().get_block_index_dims();
    std::vector<bool> rx(bidims2.get_size(), true);

    check_allowed(tns.c_str(), "el2", el2, rx);
}

/** \test Double merge of 4 dim of a 4-space on a 2-space (complex rule)
 **/
void so_merge_se_label_test::test_2n2nn_2(const std::string &table_id,
        bool product) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_2n2nn_2("
            << table_id << "," << product << ")";
    std::string tns(tnss.str());

    typedef se_label<4, double> se4_t;
    typedef se_label<2, double> se2_t;
    typedef so_merge<4, 2, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se2_t> so_merge_se_t;

    index<4> i1a, i1b;
    i1b[0] = 3; i1b[1] = 3; i1b[2] = 3; i1b[3] = 3;
    dimensions<4> bidims1(index_range<4>(i1a, i1b));

    se4_t el1(bidims1, table_id);
    {
        block_labeling<4> &bl1 = el1.get_labeling();
        mask<4> m1; m1[0] = m1[1] = m1[2] = m1[3] = true;
        for (size_t i = 0; i < 4; i++) bl1.assign(m1, i, i);
        evaluation_rule<4> r1;
        sequence<4, size_t> seq1a(0), seq1b(0);
        seq1a[0] = seq1a[1] = 1;
        seq1b[2] = seq1b[3] = 1;
        product_rule<4> &pr1 = r1.new_product();
        pr1.add(seq1a, 2);
        if (product) pr1.add(seq1b, 2);
        else {
            product_rule<4> &pr2 = r1.new_product();
            pr2.add(seq1b, 2);
        }
        el1.set_rule(r1);
    }

    symmetry_element_set<4, double> set1(se4_t::k_sym_type);
    symmetry_element_set<2, double> set2(se2_t::k_sym_type);

    set1.insert(el1);
    mask<4> m;
    m[0] = m[1] = m[2] = m[3] = true;
    sequence<4, size_t> seq(0); seq[1] = seq[3] = 1;
    symmetry_operation_params<so_merge_t> params(set1, m, seq, set2);

    so_merge_se_t().perform(params);

    if(set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected a non-empty set.");
    }

    symmetry_element_set_adapter<2, double, se2_t> adapter(set2);
    symmetry_element_set_adapter<2, double, se2_t>::iterator it =
            adapter.begin();
    const se2_t &el2 = adapter.get_elem(it);
    it++;
    if(it != adapter.end()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected only one element.");
    }

    const dimensions<2> &bidims2 = el2.get_labeling().get_block_index_dims();
    std::vector<bool> rx(bidims2.get_size(), false);

    rx[2] = rx[7] = rx[8] = rx[13] = true;
    check_allowed(tns.c_str(), "el2", el2, rx);
}

/** \test Double merge of 4 dim of a 5-space on a 3-space.
 **/
void so_merge_se_label_test::test_nmk_1(const std::string &table_id,
        bool product) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_nmk_1("
            << table_id << "," << product << ")";
    std::string tns(tnss.str());

    typedef se_label<5, double> se5_t;
    typedef se_label<3, double> se3_t;
    typedef so_merge<5, 2, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se3_t> so_merge_se_t;

    index<5> i1a, i1b;
    i1b[0] = 3; i1b[1] = 3; i1b[2] = 3; i1b[3] = 3; i1b[4] = 3;
    dimensions<5> bidims1(index_range<5>(i1a, i1b));

    se5_t el1(bidims1, table_id);
    {
        block_labeling<5> &bl1 = el1.get_labeling();
        mask<5> m1; m1[0] = m1[1] = m1[2] = m1[3] = m1[4] = true;
        for (size_t i = 0; i < 4; i++) bl1.assign(m1, i, i);

        evaluation_rule<5> r1;
        sequence<5, size_t> seq1a, seq1b;
        seq1a[0] = seq1a[1] = 1;
        seq1b[2] = seq1b[3] = seq1b[4] = 1;

        product_rule<5> &pr1 = r1.new_product();
        pr1.add(seq1a, 2);
        if (product) pr1.add(seq1b, 3);
        else {
            product_rule<5> &pr2 = r1.new_product();
            pr2.add(seq1b, 3);
        }

        el1.set_rule(r1);
    }

    symmetry_element_set<5, double> set1(se5_t::k_sym_type);
    symmetry_element_set<3, double> set2(se3_t::k_sym_type);

    set1.insert(el1);
    mask<5> m; m[0] = m[1] = m[2] = m[3] = true;
    sequence<5, size_t> seq(0); seq[2] = seq[3] = 1;
    symmetry_operation_params<so_merge_t> params(set1, m, seq, set2);

    so_merge_se_t().perform(params);

    if(set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected a non-empty set.");
    }

    symmetry_element_set_adapter<3, double, se3_t> adapter(set2);
    symmetry_element_set_adapter<3, double, se3_t>::iterator it =
            adapter.begin();
    const se3_t &el2 = adapter.get_elem(it);
    it++;
    if(it != adapter.end()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected only one element.");
    }

    const dimensions<3> &bidims2 = el2.get_labeling().get_block_index_dims();
    std::vector<bool> rx(bidims2.get_size(), false);

    if (! product) {
        index<3> idx;
        for (size_t i = 0; i < 4; i++) {
            idx[0] = i;
            idx[2] = 3;
            for (size_t j = 0; j < 4; j++) {
                idx[1] = j;
                rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
            }
            idx[2] = 2; idx[1] = 1;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
            idx[2] = 2; idx[1] = 3;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
        }
    }
    check_allowed(tns.c_str(), "el2", el2, rx);
}

/** \test Double merge of 4 dim of a 5-space on a 3-space.
 **/
void so_merge_se_label_test::test_nmk_2(const std::string &table_id,
        bool product) throw(libtest::test_exception) {

    std::ostringstream tnss;
    tnss << "so_merge_se_label_test::test_nmk_2("
            << table_id << "," << product << ")";
    std::string tns(tnss.str());

    typedef se_label<5, double> se5_t;
    typedef se_label<3, double> se3_t;
    typedef so_merge<5, 2, double> so_merge_t;
    typedef symmetry_operation_impl<so_merge_t, se3_t> so_merge_se_t;

    index<5> i1a, i1b;
    i1b[0] = 3; i1b[1] = 3; i1b[2] = 3; i1b[3] = 3; i1b[4] = 3;
    dimensions<5> bidims1(index_range<5>(i1a, i1b));

    se5_t el1(bidims1, table_id);
    {
        block_labeling<5> &bl1 = el1.get_labeling();
        mask<5> m1; m1[0] = m1[1] = m1[2] = m1[3] = m1[4] = true;
        for (size_t i = 0; i < 4; i++) bl1.assign(m1, i, i);

        evaluation_rule<5> r1;
        sequence<5, size_t> seq1a, seq1b;
        seq1a[0] = seq1a[1] = seq1a[4] = 1;
        seq1b[2] = seq1b[3] = seq1b[4] = 1;
        product_rule<5> &pr1 = r1.new_product();
        pr1.add(seq1a, 2);
        if (product) pr1.add(seq1b, 2);
        else {
            product_rule<5> &pr2 = r1.new_product();
            pr2.add(seq1b, 2);
        }
        el1.set_rule(r1);
    }

    symmetry_element_set<5, double> set1(se5_t::k_sym_type);
    symmetry_element_set<3, double> set2(se3_t::k_sym_type);

    set1.insert(el1);
    mask<5> m; m[0] = m[1] = m[2] = m[3] = true;
    sequence<5, size_t> seq(0); seq[2] = seq[3] = 1;
    symmetry_operation_params<so_merge_t> params(set1, m, seq, set2);

    so_merge_se_t().perform(params);

    if(set2.is_empty()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected a non-empty set.");
    }

    symmetry_element_set_adapter<3, double, se3_t> adapter(set2);
    symmetry_element_set_adapter<3, double, se3_t>::iterator it =
            adapter.begin();
    const se3_t &el2 = adapter.get_elem(it);
    it++;
    if(it != adapter.end()) {
        fail_test(tns.c_str(), __FILE__, __LINE__,
                "Expected only one element.");
    }

    const dimensions<3> &bidims2 = el2.get_labeling().get_block_index_dims();
    std::vector<bool> rx(bidims2.get_size(), false);

    index<3> idx;
    idx[2] = 2;
    for (size_t i = 0; i < 4; i++) {
        idx[0] = i;
        for (size_t j = 0; j < 4; j++) {
            idx[1] = j;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
        }
    }
    idx[2] = 3;
    if (product) {
        idx[0] = 1; idx[1] = 1;
        rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
        idx[0] = 1; idx[1] = 3;
        rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
        idx[0] = 3; idx[1] = 1;
        rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
        idx[0] = 3; idx[1] = 3;
        rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
    }
    else {
        for (size_t i = 0; i < 4; i++) {
            idx[0] = i; idx[1] = 1;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
            idx[0] = i; idx[1] = 3;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
            idx[0] = 1; idx[1] = i;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
            idx[0] = 3; idx[1] = i;
            rx[abs_index<3>(idx, bidims2).get_abs_index()] = true;
        }
    }

    check_allowed(tns.c_str(), "el2", el2, rx);
}
} // namespace libtensor
