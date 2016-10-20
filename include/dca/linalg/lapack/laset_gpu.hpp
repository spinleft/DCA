// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Raffaele Solca' (rasolca@itp.phys.ethz.ch)
//
// This file provides the GPU implementation of the laset function.

#ifndef DCA_LINALG_LAPACK_LASET_GPU_HPP
#define DCA_LINALG_LAPACK_LASET_GPU_HPP

#include <complex>
#include <cuComplex.h>
#include "dca/linalg/util/cast_cuda.hpp"

namespace dca {
namespace linalg {
namespace lapack {
// dca::linalg::lapack::

// Sets the diagonal elements of the matrix to diag, and the off diagonal elements to offdiag.
// Preconditions: lda >= m.
// Type can be float, double, cuComplex, cuDoubleComplex, std::complex<float>, std::complex<double>.
template <typename Type>
void laset_gpu(int m, int n, Type offdiag, Type diag, Type* a, int lda, int thread_id, int stream_id);
template <typename Type>
inline void laset_gpu(int m, int n, std::complex<Type> offdiag, std::complex<Type> diag,
                      std::complex<Type>* a, int lda, int thread_id, int stream_id) {
  auto cu_offdiag = util::castCudaComplex(offdiag);
  auto cu_diag = util::castCudaComplex(&diag);
  auto cu_a = util::castCudaComplex(a);
  laset_gpu(m, n, *cu_offdiag, *cu_diag, cu_a, lda, thread_id, stream_id);
}

}  // lapack
}  // linalg
}  // dca

#endif  // DCA_LINALG_LAPACK_LASET_GPU_HPP