#ifndef __LIBTENSOR_TENSOR_OPERATION_BASE_H
#define __LIBTENSOR_TENSOR_OPERATION_BASE_H

#include "defs.h"
#include "exception.h"
#include "permutation.h"
#include "tensor_operation_dispatcher.h"
#include "tensor_operation_i.h"

namespace libtensor {

/**	\brief Base class for tensor operations
**/
template<typename T>
class tensor_operation_base : public tensor_operation_i<T> {
public:
	typedef T element_t; //!< Tensor element type

protected:
	element_t *req_dataptr(tensor_i<element_t> &t, const permutation &p)
		throw(exception);

	void ret_dataptr(tensor_i<element_t> &t, const element_t *p)
		throw(exception);
};

template<typename T>
inline T *tensor_operation_base<T>::req_dataptr(tensor_i<T> &t,
	const permutation &p) throw(exception) {
	return tensor_operation_dispatcher<T>::get_instance().req_dataptr(t, p);
}

template<typename T>
inline void tensor_operation_base<T>::ret_dataptr(tensor_i<T> &t,
	const T *ptr) throw(exception) {
	tensor_operation_dispatcher<T>::get_instance().ret_dataptr(t, ptr);
}

} // namespace libtensor

#endif // __LIBTENSOR_TENSOR_OPERATION_BASE_H

