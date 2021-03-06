#include <libtensor/dense_tensor/dense_tensor.h>
#include <libtensor/dense_tensor/tod_random.h>
#include <libtensor/ctf_dense_tensor/ctf.h>
#include <libtensor/ctf_dense_tensor/ctf_dense_tensor.h>
#include <libtensor/ctf_dense_tensor/ctf_tod_collect.h>
#include <libtensor/ctf_dense_tensor/ctf_tod_distribute.h>
#include <libtensor/ctf_dense_tensor/ctf_tod_set.h>
#include "../compare_ref.h"
#include "ctf_tod_set_test.h"

namespace libtensor {


void ctf_tod_set_test::perform() throw(libtest::test_exception) {

    ctf::init();

    try {

        test_1(true);
        test_1(false);
        test_2(false, 0.0);
        test_2(false, -0.1);
        test_2(true, 0.0);
        test_2(true, -0.1);
        test_3(false, 0.0);
        test_3(false, 2.0);
        test_3(true, 0.0);
        test_3(true, 2.0);

    } catch(...) {
        ctf::exit();
        throw;
    }

    ctf::exit();
}


void ctf_tod_set_test::test_1(bool zero) {

    std::stringstream tnss;
    tnss << "ctf_tod_set_test::test_1(" << zero << ")";
    std::string tn = tnss.str();
    const char *testname = tn.c_str();

    typedef allocator<double> allocator_t;

    try {

    index<2> i1, i2;
    i2[0] = 99; i2[1] = 99;
    dimensions<2> dimsa(index_range<2>(i1, i2));
    dense_tensor<2, double, allocator_t> ta(dimsa), ta_ref(dimsa);
    ctf_dense_tensor<2, double> dta(dimsa);

    tod_random<2>().perform(ta);
    ctf_tod_distribute<2>(ta).perform(dta);

    if(zero) {
        tod_set<2>().perform(zero, ta_ref);
        ctf_tod_set<2>().perform(zero, dta);
    } else {
        tod_set<2>(100.0).perform(true, ta_ref);
        tod_set<2>().perform(zero, ta_ref);
        ctf_tod_set<2>(100.0).perform(true, dta);
        ctf_tod_set<2>().perform(zero, dta);
    }

    ctf_tod_collect<2>(dta).perform(ta);

    compare_ref<2>::compare(testname, ta, ta_ref, 1e-15);

    } catch(exception &e) {
        fail_test(testname, __FILE__, __LINE__, e.what());
    }
}


void ctf_tod_set_test::test_2(bool zero, double d) {

    std::stringstream tnss;
    tnss << "ctf_tod_set_test::test_2(" << zero << ", " << d << ")";
    std::string tn = tnss.str();
    const char *testname = tn.c_str();

    typedef allocator<double> allocator_t;

    try {

    index<2> i1, i2;
    i2[0] = 99; i2[1] = 99;
    dimensions<2> dimsa(index_range<2>(i1, i2));
    dense_tensor<2, double, allocator_t> ta(dimsa), ta_ref(dimsa);
    ctf_dense_tensor<2, double> dta(dimsa);

    tod_random<2>().perform(ta);
    ctf_tod_distribute<2>(ta).perform(dta);

    if(zero) {
        tod_set<2>(d).perform(zero, ta_ref);
        ctf_tod_set<2>(d).perform(zero, dta);
    } else {
        tod_set<2>(100.0).perform(true, ta_ref);
        tod_set<2>(d).perform(zero, ta_ref);
        ctf_tod_set<2>(100.0).perform(true, dta);
        ctf_tod_set<2>(d).perform(zero, dta);
    }

    ctf_tod_collect<2>(dta).perform(ta);

    compare_ref<2>::compare(testname, ta, ta_ref, 1e-15);

    } catch(exception &e) {
        fail_test(testname, __FILE__, __LINE__, e.what());
    }
}


void ctf_tod_set_test::test_3(bool zero, double d) {

    std::stringstream tnss;
    tnss << "ctf_tod_set_test::test_3(" << zero << ", " << d << ")";
    std::string tn = tnss.str();
    const char *testname = tn.c_str();

    typedef allocator<double> allocator_t;

    try {

    index<4> i1, i2;
    i2[0] = 9; i2[1] = 9; i2[2] = 9; i2[3] = 9;
    dimensions<4> dimsa(index_range<4>(i1, i2));
    dense_tensor<4, double, allocator_t> ta(dimsa), ta_ref(dimsa);
    ctf_dense_tensor<4, double> dta(dimsa);

    tod_random<4>().perform(ta);
    ctf_tod_distribute<4>(ta).perform(dta);

    if(zero) {
        tod_set<4>(d).perform(zero, ta_ref);
        ctf_tod_set<4>(d).perform(zero, dta);
    } else {
        tod_set<4>(100.0).perform(true, ta_ref);
        tod_set<4>(d).perform(zero, ta_ref);
        ctf_tod_set<4>(100.0).perform(true, dta);
        ctf_tod_set<4>(d).perform(zero, dta);
    }

    ctf_tod_collect<4>(dta).perform(ta);

    compare_ref<4>::compare(testname, ta, ta_ref, 1e-15);

    } catch(exception &e) {
        fail_test(testname, __FILE__, __LINE__, e.what());
    }
}


} // namespace libtensor

