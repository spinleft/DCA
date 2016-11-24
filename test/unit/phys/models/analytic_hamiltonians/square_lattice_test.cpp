// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Urs R. Haehner (haehneru@itp.phys.ethz.ch)
//
// This file tests square_lattice.hpp.

#include "dca/phys/models/analytic_hamiltonians/square_lattice.hpp"

#include <cmath>
#include <complex>
#include <vector>

#include "gtest/gtest.h"

#include "dca/function/domains.hpp"
#include "dca/function/function.hpp"
#include "dca/phys/domains/cluster/cluster_domain.hpp"
#include "dca/phys/domains/cluster/cluster_domain_initializer.hpp"
#include "dca/phys/domains/cluster/symmetries/point_groups/2d/2d_square.hpp"
#include "dca/phys/parameters/model_parameters.hpp"

using namespace dca;

TEST(SquareLatticeTest, Initialize_H_0) {
  using PointGroup = phys::domains::D4;
  using Lattice = phys::models::square_lattice<PointGroup>;

  using BandDmn = func::dmn<1, int>;
  using SpinDmn = func::dmn<2, int>;
  using BandSpinDmn = func::dmn_variadic<func::dmn_0<BandDmn>, func::dmn_0<SpinDmn>>;

  using KDmn = func::dmn<4, std::vector<double>>;
  KDmn::set_elements({{0., 0.}, {0., M_PI}, {M_PI, 0.}, {M_PI, M_PI}});

  func::function<std::complex<double>, func::dmn_variadic<BandSpinDmn, BandSpinDmn, func::dmn_0<KDmn>>> H_0;

  phys::params::ModelParameters<phys::models::TightBindingModel<Lattice>> params;
  params.set_t(1.);
  params.set_t_prime(0.5);

  Lattice::initialize_H_0(params, H_0);

  // All imaginary parts should be zero.
  for (int b1 = 0; b1 < BandDmn::dmn_size(); ++b1)
    for (int s1 = 0; s1 < SpinDmn::dmn_size(); ++s1)
      for (int b2 = 0; b2 < BandDmn::dmn_size(); ++b2)
        for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
          for (int k = 0; k < KDmn::dmn_size(); ++k)
            EXPECT_DOUBLE_EQ(0., H_0(b1, s1, b2, s2, k).imag());

  // All matrix elements with different spin indices should be zero.
  for (int b1 = 0; b1 < BandDmn::dmn_size(); ++b1)
    for (int b2 = 0; b2 < BandDmn::dmn_size(); ++b2)
      for (int k = 0; k < KDmn::dmn_size(); ++k) {
        EXPECT_DOUBLE_EQ(0., H_0(b1, 0, b2, 1, k).real());
        EXPECT_DOUBLE_EQ(0., H_0(b1, 1, b2, 0, k).real());
      }

  // Check nonvanishing Hamiltonian matrix elements.
  EXPECT_DOUBLE_EQ(-6., H_0(0, 0, 0, 0, 0).real());
  EXPECT_DOUBLE_EQ(-6., H_0(0, 1, 0, 1, 0).real());
  EXPECT_DOUBLE_EQ(2., H_0(0, 0, 0, 0, 1).real());
  EXPECT_DOUBLE_EQ(2., H_0(0, 1, 0, 1, 1).real());
  EXPECT_DOUBLE_EQ(2., H_0(0, 0, 0, 0, 2).real());
  EXPECT_DOUBLE_EQ(2., H_0(0, 1, 0, 1, 2).real());
  EXPECT_DOUBLE_EQ(2., H_0(0, 0, 0, 0, 3).real());
  EXPECT_DOUBLE_EQ(2., H_0(0, 1, 0, 1, 3).real());
}

TEST(SquareLatticeTest, Initialize_H_interaction) {
  using PointGroup = phys::domains::D4;
  using Lattice = phys::models::square_lattice<PointGroup>;

  using BandDmn = func::dmn_0<func::dmn<1, int>>;
  using SpinDmn = func::dmn_0<func::dmn<2, int>>;
  using BandSpinDmn = func::dmn_variadic<BandDmn, SpinDmn>;

  using r_DCA_ClusterType =
      phys::domains::cluster_domain<double, Lattice::DIMENSION, phys::domains::CLUSTER,
                                    phys::domains::REAL_SPACE, phys::domains::BRILLOUIN_ZONE>;
  using r_DCA = func::dmn_0<r_DCA_ClusterType>;

  const std::vector<std::vector<int>> DCA_cluster{{-2, 0}, {0, 2}};
  phys::domains::cluster_domain_initializer<r_DCA>::execute(Lattice::initialize_r_DCA_basis(),
                                                            DCA_cluster);

  // Get index of origin and check it.
  const int origin = r_DCA_ClusterType::origin_index();
  ASSERT_DOUBLE_EQ(0., r_DCA::get_elements()[origin][0]);
  ASSERT_DOUBLE_EQ(0., r_DCA::get_elements()[origin][1]);

  std::vector<typename r_DCA_ClusterType::element_type>& basis =
      r_DCA_ClusterType::get_basis_vectors();
  std::vector<typename r_DCA_ClusterType::element_type>& super_basis =
      r_DCA_ClusterType::get_super_basis_vectors();
  std::vector<typename r_DCA_ClusterType::element_type>& elements = r_DCA_ClusterType::get_elements();

  // Check that the super_basis is equal to the input (DCA_cluster).
  ASSERT_EQ(-2., super_basis[0][0]);
  ASSERT_EQ(0., super_basis[0][1]);
  ASSERT_EQ(0., super_basis[1][0]);
  ASSERT_EQ(2., super_basis[1][1]);

  ASSERT_EQ(2, basis.size());

  // Compute indices of nearest neighbours w.r.t. origin.
  std::vector<int> nn_index(2);  // There are two different nearest neighbour pairs.
  for (int d = 0; d < 2; ++d) {
    std::vector<double> basis_vec =
        phys::domains::cluster_operations::translate_inside_cluster(basis[d], super_basis);
    nn_index[d] =
        phys::domains::cluster_operations::index(basis_vec, elements, phys::domains::BRILLOUIN_ZONE);
  }

  func::function<double, func::dmn_variadic<BandSpinDmn, BandSpinDmn, r_DCA>> H_interaction;
  phys::params::ModelParameters<phys::models::TightBindingModel<Lattice>> params;

  // Check on-site interaction.
  params.set_U(4);
  params.set_V(0);
  params.set_V_prime(0);

  Lattice::initialize_H_interaction(H_interaction, params);

  for (int r = 0; r < r_DCA::dmn_size(); ++r)
    for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
      for (int s1 = 0; s1 < SpinDmn::dmn_size(); ++s1)
        if (r == origin && s1 != s2)
          EXPECT_DOUBLE_EQ(4., H_interaction(0, s1, 0, s2, r));
        else
          EXPECT_DOUBLE_EQ(0., H_interaction(0, s1, 0, s2, r));

  // Check nearest-neighbor opposite spin interaction.
  params.set_U(0);
  params.set_V(2);
  params.set_V_prime(0);

  Lattice::initialize_H_interaction(H_interaction, params);

  for (int r = 0; r < r_DCA::dmn_size(); ++r)
    for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
      for (int s1 = 0; s1 < SpinDmn::dmn_size(); ++s1)
        if (std::find(nn_index.begin(), nn_index.end(), r) != nn_index.end() && s1 != s2)
          EXPECT_DOUBLE_EQ(2., H_interaction(0, s1, 0, s2, r));
        else
          EXPECT_DOUBLE_EQ(0., H_interaction(0, s1, 0, s2, r));

  // Check nearest-neighbor same spin interaction.
  params.set_U(0);
  params.set_V(0);
  params.set_V_prime(1);

  Lattice::initialize_H_interaction(H_interaction, params);

  for (int r = 0; r < r_DCA::dmn_size(); ++r)
    for (int s2 = 0; s2 < SpinDmn::dmn_size(); ++s2)
      for (int s1 = 0; s1 < SpinDmn::dmn_size(); ++s1)
        if (std::find(nn_index.begin(), nn_index.end(), r) != nn_index.end() && s1 == s2)
          EXPECT_DOUBLE_EQ(1., H_interaction(0, s1, 0, s2, r));
        else
          EXPECT_DOUBLE_EQ(0., H_interaction(0, s1, 0, s2, r));
}
