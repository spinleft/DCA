// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
// See LICENSE.txt for terms of usage./
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Giovanni Balduzzi (gbalduzz@itp.phys.ethz.ch)
//
//

#ifndef DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_ACCUMULATOR_CTINT_ACCUMULATOR_CONFIGURATION_HPP
#define DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_ACCUMULATOR_CTINT_ACCUMULATOR_CONFIGURATION_HPP

#include <array>

#include "dca/linalg/matrix.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/structs/ct_int_matrix_configuration.hpp"


namespace dca {
namespace phys {
namespace solver {
namespace ctint {
// dca::phys::solver::ctint::

struct AccumulatorConfiguration {
  int sign = 0;
  std::array<linalg::Matrix<double, linalg::CPU>, 2> M;
  ctint::MatrixConfiguration matrix_configuration;

  int size() const {
    return matrix_configuration.size();
  }
};

}  // ctint
}  // solver
}  // phys
}  // dca

#endif  // DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_ACCUMULATOR_CTINT_ACCUMULATOR_CONFIGURATION_HPP
