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
//
// TODO: Split up in one file per enum and maybe remove IS_EQUAL and GENERIC_ASSERT structs.

#ifndef MATH_LIBRARY_FUNCTIONAL_TRANSFORMS_TYPEDEFS_HPP
#define MATH_LIBRARY_FUNCTIONAL_TRANSFORMS_TYPEDEFS_HPP

#include <string>

namespace math_algorithms {

enum DOMAIN_REPRESENTATIONS { DISCRETE, CONTINUOUS, EXPANSION };

std::string to_str(DOMAIN_REPRESENTATIONS DR) {
  switch (DR) {
    case DISCRETE:
      return "DISCRETE";

    case CONTINUOUS:
      return "CONTINUOUS";

    case EXPANSION:
      return "EXPANSION";

    default:
      return "NOT DEFINED";
  }
}

enum BASIS_EXPANSIONS {
  KRONECKER_DELTA,

  HARMONICS,
  COSINE,
  SINE,

  HERMITE_LINEAR_SPLINE,
  HERMITE_CUBIC_SPLINE,

  LEGENDRE_P,
  LEGENDRE_Q,
  LEGENDRE_LM
};

std::string to_str(BASIS_EXPANSIONS BS) {
  switch (BS) {
    case KRONECKER_DELTA:
      return "KRONECKER_DELTA";

    case HARMONICS:
      return "HARMONICS";

    case SINE:
      return "SINE";

    case COSINE:
      return "COSINE";

    case HERMITE_LINEAR_SPLINE:
      return "HERMITE_LINEAR_SPLINE";

    case HERMITE_CUBIC_SPLINE:
      return "HERMITE_CUBIC_SPLINE";

    case LEGENDRE_P:
      return "LEGENDRE_P";

    case LEGENDRE_Q:
      return "LEGENDRE_Q";

    case LEGENDRE_LM:
      return "LEGENDRE_LM";

    default:
      return "NOT DEFINED";
  }
}

enum BOUNDARY_CONDITIONS { INTERVAL, PERIODIC, ANTIPERIODIC };

std::string to_str(BOUNDARY_CONDITIONS BC) {
  switch (BC) {
    case INTERVAL:
      return "INTERVAL";

    case PERIODIC:
      return "PERIODIC";

    case ANTIPERIODIC:
      return "ANTIPERIODIC";

    default:
      return "NOT DEFINED";
  }
}

enum ELEMENT_SPACINGS { EQUIDISTANT, NONEQUIDISTANT };

std::string to_str(ELEMENT_SPACINGS IS) {
  switch (IS) {
    case EQUIDISTANT:
      return "EQUIDISTANT";

    case NONEQUIDISTANT:
      return "NONEQUIDISTANT";

    default:
      return "NOT DEFINED";
  }
}

template <typename type_lhs, typename type_rhs>
struct IS_EQUAL {
  const static bool CHECK = false;
};

template <typename type_lhs>
struct IS_EQUAL<type_lhs, type_lhs> {
  const static bool CHECK = true;
};

template <bool QUESTION>
struct GENERIC_ASSERT {};

template <>
struct GENERIC_ASSERT<false> {};

template <>
struct GENERIC_ASSERT<true> {
  static void execute(){};
};

}  // math_algorithms

#endif  // MATH_LIBRARY_FUNCTIONAL_TRANSFORMS_TYPEDEFS_HPP
