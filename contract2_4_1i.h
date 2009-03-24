#ifndef LIBTENSOR_CONTRACT2_4_1I_H
#define LIBTENSOR_CONTRACT2_4_1I_H

#include "defs.h"
#include "exception.h"
#include "dimensions.h"
#include "permutation.h"

namespace libtensor {

/**	\brief Contracts a second-order %tensor with a fourth-order %tensor
		over one %index.

	Performs contraction:
	\f[ c_{ijkl} = \mathcal{P}_c \sum_{m}
		\mathcal{P}_a a_{im} \mathcal{P}_b b_{jklm} \f]

	\ingroup libtensor_tod
**/
class contract2_4_1i {
public:
	static void contract(double *c, const dimensions<4> &dc,
		const permutation<4> &pc, const double *a,
		const dimensions<2> &da, const permutation<2> &pa,
		const double *b, const dimensions<4> &db,
		const permutation<4> &pb) throw(exception);

private:
	/**	\brief \f$ c_{jikl} = \sum_{m} a_{mi} b_{jmkl} \f$
	**/
	static void c_1023_10_0231(double *c, const dimensions<4> &dc,
		const double *a, const dimensions<2> &da,
		const double *b, const dimensions<4> &db);
};

} // namespace libtensor

#endif // LIBTENSOR_CONTRACT2_4_1I_H
