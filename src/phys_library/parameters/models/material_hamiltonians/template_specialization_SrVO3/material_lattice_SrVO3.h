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

#ifndef PHYS_LIBRARY_PARAMETERS_MODELS_MATERIAL_HAMILTONIANS_TEMPLATE_SPECIALIZATION_SRVO3_MATERIAL_LATTICE_SRVO3_H
#define PHYS_LIBRARY_PARAMETERS_MODELS_MATERIAL_HAMILTONIANS_TEMPLATE_SPECIALIZATION_SRVO3_MATERIAL_LATTICE_SRVO3_H

#include "comp_library/IO_library/CSV/CSV.hpp"
#include "phys_library/parameters/models/material_hamiltonians/material_lattice_template.h"

template <typename point_group_type>
class material_lattice<SrVO3, point_group_type> {
public:
  const static int DIMENSION = 3;
  const static int BANDS = 12;

  typedef no_symmetry<DIMENSION> LDA_point_group;
  typedef point_group_type DCA_point_group;

  static double* initialize_r_DCA_basis();
  static double* initialize_r_LDA_basis();

  static std::vector<int> get_flavors();
  static std::vector<std::vector<double>> get_a_vectors();

  static std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> get_orbital_permutations();

  template <class domain, class parameters_type>
  static void initialize_H_interaction(FUNC_LIB::function<double, domain>& H_interaction,
                                       parameters_type& parameters);

  template <class domain>
  static void initialize_H_symmetry(FUNC_LIB::function<int, domain>& H_symmetry);

  template <class parameters_type>
  static std::complex<double> get_LDA_Hamiltonians(parameters_type& parameters, std::vector<double> k,
                                                   int b1, int s1, int b2, int s2);

private:
  template <class parameters_type>
  static std::vector<std::vector<double>>& get_t_ij(parameters_type& parameters);
};

template <typename point_group_type>
double* material_lattice<SrVO3, point_group_type>::initialize_r_DCA_basis() {
  const static double a = 7.260327248;

  static double* r_DCA = new double[9];

  r_DCA[0] = a;
  r_DCA[3] = 0;
  r_DCA[6] = 0;
  r_DCA[1] = 0;
  r_DCA[4] = a;
  r_DCA[7] = 0;
  r_DCA[2] = 0;
  r_DCA[5] = 0;
  r_DCA[8] = a;

  return r_DCA;
}

template <typename point_group_type>
double* material_lattice<SrVO3, point_group_type>::initialize_r_LDA_basis() {
  const static double a = 7.260327248;

  static double* r_LDA = new double[9];

  r_LDA[0] = a;
  r_LDA[3] = 0;
  r_LDA[6] = 0;
  r_LDA[1] = 0;
  r_LDA[4] = a;
  r_LDA[7] = 0;
  r_LDA[2] = 0;
  r_LDA[5] = 0;
  r_LDA[8] = a;

  return r_LDA;
}

template <typename point_group_type>
std::vector<int> material_lattice<SrVO3, point_group_type>::get_flavors() {
  static std::vector<int> flavors(BANDS);
  bool sym = true;
  if (sym) {
    flavors[0] = 0;
    flavors[1] = 0;
    flavors[2] = 0;
    flavors[3] = 1;
    flavors[4] = 1;
    flavors[5] = 2;
    flavors[6] = 2;
    flavors[7] = 1;
    flavors[8] = 1;
    flavors[9] = 1;
    flavors[10] = 2;
    flavors[11] = 1;
  }
  else {
    for (int i = 0; i < 12; i++)
      flavors[i] = i;
  }
  return flavors;
}

template <typename point_group_type>
std::vector<std::vector<double>> material_lattice<SrVO3, point_group_type>::get_a_vectors() {
  static std::vector<std::vector<double>> a_vecs(BANDS, std::vector<double>(DIMENSION, 0.));
  return a_vecs;
}

template <typename point_group_type>
std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> material_lattice<
    SrVO3, point_group_type>::get_orbital_permutations() {
  static std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> permutations(0);
  return permutations;
}

template <typename point_group_type>
template <class domain, class parameters_type>
void material_lattice<SrVO3, point_group_type>::initialize_H_interaction(
    FUNC_LIB::function<double, domain>& H_i, parameters_type& parameters) {
  std::vector<std::vector<double>> U;

  IO::reader<IO::CSV>::execute(parameters.get_U_ij_file_name(), U);

  for (int i = 0; i < 2 * BANDS; i++)
    for (int j = 0; j < 2 * BANDS; j++)
      H_i(i, j, 0) = U[i][j];

  /*
  {
    std::stringstream ss;
    ss.precision(6);
    ss<<scientific;

    ss << "\n\n U_ij \n\n";
    for(int i=0; i<2*BANDS; i++){
      for(int j=0; j<2*BANDS; j++){
        ss << "\t" << H_i(i,j,0);
      }
      ss << "\n";
    }
    ss << "\n\n";

    cout << ss.str();
  }
  */
}

template <typename point_group_type>
template <class domain>
void material_lattice<SrVO3, point_group_type>::initialize_H_symmetry(
    FUNC_LIB::function<int, domain>& H_symmetries) {
  H_symmetries = -1;

  for (int s = 0; s < 2; s++)
    for (int i = 0; i < BANDS; i++)
      H_symmetries(i, s, i, s) = get_flavors()[i];
}

template <typename point_group_type>
template <class parameters_type>
std::vector<std::vector<double>>& material_lattice<SrVO3, point_group_type>::get_t_ij(
    parameters_type& parameters) {
  static std::vector<std::vector<double>> t_ij(0, std::vector<double>(0));

  static bool is_initialized = false;

  if (not is_initialized) {
    IO::reader<IO::CSV>::execute(parameters.get_t_ij_file_name(), t_ij);

    is_initialized = true;

    /*
    cout << "\n\n";
    for(size_t i=0; i<128; i++){
      cout << "\t";
      VECTOR_OPERATIONS::PRINT(t_ij[i]);
      cout << "\n";
    }
    */
  }

  return t_ij;
}

template <typename point_group_type>
template <class parameters_type>
std::complex<double> material_lattice<SrVO3, point_group_type>::get_LDA_Hamiltonians(
    parameters_type& parameters, std::vector<double> k, int b1, int s1, int b2, int s2) {
  const static std::complex<double> I(0, 1);

  double* r_LDA = initialize_r_LDA_basis();

  std::vector<std::vector<double>>& t_ij = get_t_ij(parameters);

  std::complex<double> H_LDA = 0.;

  if (s1 == s2) {
    std::complex<double> val;

    for (size_t i = 0; i < t_ij.size(); i++) {
      std::vector<double>& t = t_ij[i];

      assert(t.size() == 8);

      int b_i = t[0];
      int b_j = t[1];

      if (b_i == b1 and b_j == b2) {
        double r_x = t[2] * r_LDA[0] + t[3] * r_LDA[3] + t[4] * r_LDA[6];
        double r_y = t[2] * r_LDA[1] + t[3] * r_LDA[4] + t[4] * r_LDA[7];
        double r_z = t[2] * r_LDA[2] + t[3] * r_LDA[5] + t[4] * r_LDA[8];

        val.real(t[5]);
        val.imag(t[6]);

        double multiplicity = t[7];

        H_LDA += val * std::exp(I * (k[0] * r_x + k[1] * r_y + k[2] * r_z)) / multiplicity;
      }
    }
  }

  return H_LDA;
}

#endif  // PHYS_LIBRARY_PARAMETERS_MODELS_MATERIAL_HAMILTONIANS_TEMPLATE_SPECIALIZATION_SRVO3_MATERIAL_LATTICE_SRVO3_H
