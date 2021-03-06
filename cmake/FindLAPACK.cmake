set(LAPACK_FOUND FALSE)

find_library(LAPACK_LIBRARY_PATH lapack)

if(LAPACK_LIBRARY_PATH)
set(LAPACK_FOUND TRUE)
set(LAPACK_LIBRARIES LAPACK_LIBRARY_PATH)

if(NOT LAPACK_FIND_QUIETLY)
	message(STATUS "Found generic LAPACK: " ${LAPACK_LIBRARY_PATH})
endif(NOT LAPACK_FIND_QUIETLY)

endif(LAPACK_LIBRARY_PATH)

