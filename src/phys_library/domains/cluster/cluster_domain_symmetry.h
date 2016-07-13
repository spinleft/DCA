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

#ifndef PHYS_LIBRARY_DOMAINS_CLUSTER_CLUSTER_DOMAIN_SYMMETRY_H
#define PHYS_LIBRARY_DOMAINS_CLUSTER_CLUSTER_DOMAIN_SYMMETRY_H

#include "comp_library/function_library/include_function_library.h"
#include "phys_library/domains/cluster/cluster_domain.h"
#include "phys_library/domains/cluster/cluster_domain_family.h"
#include "phys_library/domains/cluster/cluster_typedefs.hpp"
#include "phys_library/domains/Quantum_domain/electron_band_domain.h"
#include "phys_library/domains/Quantum_domain/point_group_operation_dmn.h"

template <typename scalar_type, int D, CLUSTER_NAMES N, CLUSTER_REPRESENTATION R, CLUSTER_SHAPE S>
class cluster_symmetry<cluster_domain<scalar_type, D, N, R, S>> {
  const static int DIMENSION = D;

  const static CLUSTER_NAMES NAME = N;
  const static CLUSTER_REPRESENTATION REPRESENTATION = R;
  const static CLUSTER_SHAPE SHAPE = S;

  const static CLUSTER_REPRESENTATION DUAL_REPRESENTATION =
      dual_cluster<REPRESENTATION>::REPRESENTATION;

public:
  typedef cluster_domain_family<scalar_type, D, N, S> cluster_family_type;

  typedef cluster_domain<scalar_type, D, N, REPRESENTATION, S> this_type;
  typedef cluster_domain<scalar_type, D, N, DUAL_REPRESENTATION, S> dual_type;

  typedef dmn_0<electron_band_domain> b_dmn_t;
  typedef dmn_0<this_type> c_dmn_t;

  typedef dmn_0<point_group_symmetry_domain<UNIT_CELL, cluster_family_type>> sym_unit_cell_dmn_t;
  typedef dmn_0<point_group_symmetry_domain<SUPER_CELL, cluster_family_type>> sym_super_cell_dmn_t;

  typedef dmn_2<dmn_2<c_dmn_t, b_dmn_t>, sym_super_cell_dmn_t> symmetry_matrix_dmn_t;

public:
  static FUNC_LIB::function<std::pair<int, int>, symmetry_matrix_dmn_t>& get_symmetry_matrix() {
    static FUNC_LIB::function<std::pair<int, int>, symmetry_matrix_dmn_t> symmetry_matrix(
        "symmetry_matrix_super_cell");
    return symmetry_matrix;
  }
};

#endif  // PHYS_LIBRARY_DOMAINS_CLUSTER_CLUSTER_DOMAIN_SYMMETRY_H
