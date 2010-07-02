#ifndef LIBTENSOR_BTOD_PRINT_H
#define LIBTENSOR_BTOD_PRINT_H

#include <ostream>
#include <iomanip>
#include <libvmm/libvmm.h>
#include "../defs.h"
#include "../exception.h"
#include "../core/block_tensor_i.h"
#include "../core/tensor.h"
#include "../core/tensor_ctrl.h"
#include "../tod/tod_btconv.h"

namespace libtensor {

template<size_t N>
class btod_print {
public:
	static const char *k_clazz; //!< Class name

private:
	std::ostream &m_stream; //!< Output stream
	size_t m_precision; //!< Precision of the output
	bool m_sci; //!< Set the scientific flag on output

public:
	btod_print(std::ostream &stream, size_t pre = 16, bool sci = true) :
		m_stream(stream), m_precision(pre), m_sci(sci) { }

	void perform(block_tensor_i<N,double> &bt) throw(exception);
};

template<size_t N>
const char *btod_print<N>::k_clazz = "btod_print<N>";


template<size_t N>
void btod_print<N>::perform(block_tensor_i<N,double> &bt) throw(exception)
{
	static const char *method = "perform(block_tensor_i<N, double>&)";

	if(! m_stream.good()) {
		throw_exc(k_clazz, method, "Output stream not initialized.");
	}

	const dimensions<N> &dims = bt.get_bis().get_dims();
	m_stream << N;
	for (size_t i = 0; i < N; i++) {
		m_stream << " " << dims.get_dim(i);
	}
	m_stream << std::endl;

	typedef libvmm::std_allocator<double> allocator_t;
	tensor<N, double, allocator_t> ta(dims);
	tod_btconv<N>(bt).perform(ta);

	tensor_ctrl<N, double> ctrla(ta);
	const double *cptra = ctrla.req_const_dataptr();

	size_t width;
	if (m_sci)
		width = m_precision + 9;
	else
		width = m_precision + 6;

	for (size_t i=0; i < dims.get_size(); i++) {
		m_stream << std::setw(width) << std::setprecision(m_precision);
		m_stream << std::right;
		if ( m_sci )
			m_stream << std::scientific;
		else
			m_stream << std::fixed;

		m_stream << cptra[i];

		if (i % 3 == 2)
			m_stream << std::endl;
		else
			m_stream << " ";
	}
	m_stream << std::endl;

	ctrla.ret_dataptr(cptra);
}


} // namespace libtensor


#endif // LIBTENSOR_BTOD_PRINT_H