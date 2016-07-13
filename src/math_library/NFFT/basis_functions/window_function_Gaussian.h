// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Peter Staar (taa@zurich.ibm.com)
//
// Description

#ifndef MATH_LIBRARY_NFFT_BASIS_FUNCTIONS_WINDOW_FUNCTION_GAUSSIAN_H
#define MATH_LIBRARY_NFFT_BASIS_FUNCTIONS_WINDOW_FUNCTION_GAUSSIAN_H

#include <cmath>
#include "math_library/static_functions.h"  // for square

namespace math_algorithms {
namespace NFFT {
// math_algorithms::NFFT::

struct gaussian_window_function {
  static int n;
  static int m;

  static double sigma;

  inline static double phi_t(double x);
  inline static double d_phi_t(double x);

  inline static double phi_wn(int n);

private:
  inline static double b_val();
};

int gaussian_window_function::n = 1;
int gaussian_window_function::m = 1;

double gaussian_window_function::sigma = 1;

double gaussian_window_function::phi_t(double x) {
  return 1. / std::sqrt(M_PI * b_val()) * std::exp(-square(n * x) / b_val());
}

double gaussian_window_function::d_phi_t(double x) {
  return 1. / std::sqrt(M_PI * b_val()) * (-2. * square(n) * x / b_val()) *
         std::exp(-square(n * x) / b_val());
}

double gaussian_window_function::phi_wn(int wn) {
  return 1. / double(n) * std::exp(-b_val() * square(M_PI * double(wn) / double(n)));
}

double gaussian_window_function::b_val() {
  return m / M_PI * (2 * sigma) / (2 * sigma - 1);
}

}  // NFFT
}  // math_algorithm

#endif  // MATH_LIBRARY_NFFT_BASIS_FUNCTIONS_WINDOW_FUNCTION_GAUSSIAN_H
