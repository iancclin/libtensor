#ifndef LIBTENSOR_LINALG_I_I_I_TEST_H
#define LIBTENSOR_LINALG_I_I_I_TEST_H

#include "linalg_test_base.h"

namespace libtensor {


/** \brief Tests the libtensor::linalg class (i_i_i)

    \ingroup libtensor_tests_linalg
 **/
class linalg_i_i_i_test : public linalg_test_base {
public:
    virtual void perform() throw(libtest::test_exception);

private:
    void test_i_i_i(size_t ni, size_t sia, size_t sib, size_t sic)
        throw(libtest::test_exception);

};


} // namespace libtensor

#endif // LIBTENSOR_LINALG_I_I_I_TEST_H
