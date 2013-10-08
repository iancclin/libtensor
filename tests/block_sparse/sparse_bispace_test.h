#ifndef LIBTENSOR_SPARSE_BISPACE_TEST_H 
#define LIBTENSOR_SPARSE_BISPACE_TEST_H

#include <libtest/unit_test.h>

namespace libtensor {

/** \brief Tests the libtensor::sparse_bispace class

    \ingroup libtensor_tests_sparse
 **/
class sparse_bispace_test : public libtest::unit_test {
public:
    virtual void perform() throw(libtest::test_exception);

private:

    /* 
     * sparse_bispace<1>::get_dim(...) tests
     */
    //Positive tests (produce correct result)
    void test_get_dim() throw(libtest::test_exception);

    /* 
     * sparse_bispace<1>::get_n_blocks(...) tests
     */
    //Positive tests (produce correct result)
    void test_get_n_blocks() throw(libtest::test_exception);


    /* 
     * sparse_bispace<1>::split(...) tests
     */

    //Negative tests (throw correct exception) 
    void test_split_not_strictly_increasing() throw(libtest::test_exception);
    void test_split_not_strictly_increasing_two_calls() throw(libtest::test_exception);
    void test_split_gt_upper_bound() throw(libtest::test_exception);
    void test_split_zero_size() throw(libtest::test_exception);

    /* 
     * sparse_bispace<1>::operator==(...) tests
     */
    //Positive tests (produce correct result)
    void test_equality_true() throw(libtest::test_exception);
    void test_equality_false_diff_dims() throw(libtest::test_exception);
    void test_equality_false_diff_splits() throw(libtest::test_exception);
    void test_nd_equality_true() throw(libtest::test_exception);

    /* 
     * sparse_bispace<1>::get_block_abs_index(...) tests
     */

    //Positive tests (produce correct result)
    void test_get_block_abs_index_one_block() throw(libtest::test_exception);
    void test_get_block_abs_index_two_block() throw(libtest::test_exception);
    
    //Negative tests (throw correct exception)
    void test_get_block_abs_index_gt_upper_bound() throw(libtest::test_exception);


    /* 
     * sparse_bispace<1>::get_block_size(...) tests
     */

    //Positive tests (produce correct result)
    void test_get_block_size_one_block() throw(libtest::test_exception);
    void test_get_block_size_two_block() throw(libtest::test_exception);
    
    //Negative tests (throw correct exception)
    void test_get_block_size_gt_upper_bound() throw(libtest::test_exception);  

    /*
     * sparse_bispace<N>::operator[](...) tests
     */
    //Negative tests (throw correct exception)
    void test_nd_subscript_operator_gt_upper_bound() throw(libtest::test_exception);

    /*
     * sparse_bispace<N>::operator|(...) tests
     */
    //Positive tests (produce correct result)
    void test_nd_bar_operator_both_operands_1d() throw(libtest::test_exception);
    void test_nd_bar_operator_both_operands_2d() throw(libtest::test_exception);

    /* 
     * sparse_bispace<N>::get_nnz(...) tests
     */
    //Positive tests (produce correct result)
    void test_get_nnz_dense() throw(libtest::test_exception);
};

} // namespace libtensor

#endif // LIBTENSOR_SPARSE_BISPACE_TEST_H