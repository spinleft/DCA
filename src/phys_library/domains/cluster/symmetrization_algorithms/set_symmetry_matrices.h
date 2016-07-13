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

#ifndef PHYS_LIBRARY_DOMAINS_CLUSTER_SYMMETRIZATION_ALGORITHMS_SET_SYMMETRY_MATRICES_H
#define PHYS_LIBRARY_DOMAINS_CLUSTER_SYMMETRIZATION_ALGORITHMS_SET_SYMMETRY_MATRICES_H

#include <stdexcept>
#include <vector>

#include "comp_library/function_library/include_function_library.h"
#include "math_library/geometry_library/vector_operations/vector_operations.hpp"
#include "phys_library/domains/cluster/cluster_domain_symmetry.h"
#include "phys_library/domains/cluster/cluster_operations.hpp"
#include "phys_library/domains/Quantum_domain/electron_band_domain.h"
#include "phys_library/domains/Quantum_domain/point_group_operation_dmn.h"

template <class base_cluster_type>
class set_symmetry_matrices {
  const static int DIMENSION = base_cluster_type::DIMENSION;

  //   typedef r_cluster<FULL, base_cluster_type> r_cluster_type;
  //   typedef k_cluster<FULL, base_cluster_type> k_cluster_type;

  typedef typename base_cluster_type::r_cluster_type r_cluster_type;
  typedef typename base_cluster_type::k_cluster_type k_cluster_type;

  typedef dmn_0<r_cluster_type> r_dmn_t;
  typedef dmn_0<k_cluster_type> k_dmn_t;

  typedef dmn_0<electron_band_domain> b_dmn_t;

  typedef point_group_symmetry_domain<UNIT_CELL, base_cluster_type> sym_unit_cell_t;
  typedef point_group_symmetry_domain<SUPER_CELL, base_cluster_type> sym_super_cell_t;

  typedef dmn_0<sym_unit_cell_t> sym_unit_cell_dmn_t;
  typedef dmn_0<sym_super_cell_t> sym_super_cell_dmn_t;

public:
  static void execute();

  static void print_on_shell();

private:
  static void set_r_symmetry_matrix();
  static void set_k_symmetry_matrix();

  static int find_k_index(std::vector<double> k);
};

template <class base_cluster_type>
void set_symmetry_matrices<base_cluster_type>::execute() {
  set_r_symmetry_matrix();

  set_k_symmetry_matrix();
}

template <class base_cluster_type>
void set_symmetry_matrices<base_cluster_type>::set_r_symmetry_matrix() {
  // FUNC_LIB::function<std::pair<int,int>, dmn_2< dmn_2<r_dmn_t,b_dmn_t>, sym_super_cell_dmn_t > >&
  // symmetry_matrix = r_cluster_type::get_symmetry_matrix();
  FUNC_LIB::function<std::pair<int, int>, dmn_2<dmn_2<r_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>&
      symmetry_matrix = cluster_symmetry<r_cluster_type>::get_symmetry_matrix();

  for (int i = 0; i < r_dmn_t::dmn_size(); ++i) {
    for (int j = 0; j < b_dmn_t::dmn_size(); ++j) {
      for (int l = 0; l < sym_super_cell_dmn_t::dmn_size(); ++l) {
        symmetry_matrix(i, j, l) = std::pair<int, int>(-1, -1);

        std::vector<double> r_plus_a =
            VECTOR_OPERATIONS::ADD(r_dmn_t::get_elements()[i], b_dmn_t::get_elements()[j].a_vec);
        std::vector<double> trafo_r_plus_a(DIMENSION, 0);

        sym_super_cell_dmn_t::get_elements()[l].transform(&r_plus_a[0], &trafo_r_plus_a[0]);

        // trafo_r_plus_a = r_cluster_type::back_inside_cluster(trafo_r_plus_a);
        trafo_r_plus_a = cluster_operations::translate_inside_cluster(
            trafo_r_plus_a, r_cluster_type::get_super_basis_vectors());

        for (int r_ind = 0; r_ind < r_dmn_t::dmn_size(); ++r_ind) {
          for (int b_ind = 0; b_ind < b_dmn_t::dmn_size(); ++b_ind) {
            std::vector<double> rj_plus_aj = VECTOR_OPERATIONS::ADD(
                r_dmn_t::get_elements()[r_ind], b_dmn_t::get_elements()[b_ind].a_vec);

            // rj_plus_aj = r_cluster_type::back_inside_cluster(rj_plus_aj);
            rj_plus_aj = cluster_operations::translate_inside_cluster(
                rj_plus_aj, r_cluster_type::get_super_basis_vectors());

            if (VECTOR_OPERATIONS::L2_NORM(rj_plus_aj, trafo_r_plus_a) < 1.e-6 and
                b_dmn_t::get_elements()[j].flavor == b_dmn_t::get_elements()[b_ind].flavor)
              symmetry_matrix(i, j, l) = std::pair<int, int>(r_ind, b_ind);
          }
        }

        if (symmetry_matrix(i, j, l).first == -1 or symmetry_matrix(i, j, l).second == -1) {
          std::vector<double> r_plus_a =
              VECTOR_OPERATIONS::ADD(r_dmn_t::get_elements()[i], b_dmn_t::get_elements()[j].a_vec);

          std::vector<double> trafo_r_plus_a(DIMENSION, 0);
          std::vector<double> trafo_r_plus_a_in_cluster(DIMENSION, 0);

          sym_super_cell_dmn_t::get_elements()[l].transform(&r_plus_a[0], &trafo_r_plus_a[0]);

          VECTOR_OPERATIONS::PRINT(r_plus_a);
          std::cout << "\t-->\t";
          VECTOR_OPERATIONS::PRINT(trafo_r_plus_a);
          std::cout << "\t-->\t";

          std::vector<double> r_affine = VECTOR_OPERATIONS::COORDINATES(
              trafo_r_plus_a, r_cluster_type::get_super_basis_vectors());

          VECTOR_OPERATIONS::PRINT(r_affine);
          std::cout << "\t-->\t";

          trafo_r_plus_a_in_cluster = cluster_operations::translate_inside_cluster(
              trafo_r_plus_a, r_cluster_type::get_super_basis_vectors());

          VECTOR_OPERATIONS::PRINT(trafo_r_plus_a_in_cluster);
          std::cout << "\n\n";

          sym_super_cell_dmn_t::get_elements()[l].to_JSON(std::cout);

          // assert(false);
          // throw std::logic_error(__FUNCTION__);
        }
      }
    }
  }
}

template <class base_cluster_type>
void set_symmetry_matrices<base_cluster_type>::set_k_symmetry_matrix() {
  FUNC_LIB::function<std::pair<int, int>,
                     dmn_2<dmn_2<r_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>& r_symmetry_matrix =
      cluster_symmetry<r_cluster_type>::get_symmetry_matrix();  // r_cluster_type::get_symmetry_matrix();
  FUNC_LIB::function<std::pair<int, int>,
                     dmn_2<dmn_2<k_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>& k_symmetry_matrix =
      cluster_symmetry<k_cluster_type>::get_symmetry_matrix();  // k_cluster_type::get_symmetry_matrix();

  //   r_symmetry_matrix.print_fingerprint();
  //   k_symmetry_matrix.print_fingerprint();

  for (int i = 0; i < k_dmn_t::dmn_size(); ++i) {
    for (int j = 0; j < b_dmn_t::dmn_size(); ++j) {
      for (int l = 0; l < sym_super_cell_dmn_t::dmn_size(); ++l) {
        std::vector<double> k = k_dmn_t::get_elements()[i];
        std::vector<double> trafo_k(DIMENSION, 0);

        sym_super_cell_dmn_t::get_elements()[l].linear_transform(&k[0], &trafo_k[0]);

        k_symmetry_matrix(i, j, l).first = find_k_index(trafo_k);
        k_symmetry_matrix(i, j, l).second = r_symmetry_matrix(i, j, l).second;
      }
    }
  }
}

template <class base_cluster_type>
int set_symmetry_matrices<base_cluster_type>::find_k_index(std::vector<double> k) {
  /*
    assert(k_cluster_type::test_back_inside_cluster());

    int index=-1;

    //k = k_cluster_type::back_inside_cluster(k);
    k = cluster_operations::translate_inside_cluster(k, k_cluster_type::get_super_basis_new());

    for(int k_ind=0; k_ind<k_dmn_t::dmn_size(); ++k_ind)
    if(VECTOR_OPERATIONS::L2_NORM(k_dmn_t::get_elements()[k_ind], k)<1.e-6)
    index = k_ind;

    if(index<0)
    throw std::logic_error(__FUNCTION__);

    return index;
  */

  k = cluster_operations::translate_inside_cluster(k, k_cluster_type::get_super_basis_vectors());

  int index = cluster_operations::index(k, k_cluster_type::get_elements(), base_cluster_type::SHAPE);

  assert(index > -1 and index < k_cluster_type::get_elements().size());
  assert(VECTOR_OPERATIONS::L2_NORM(k_dmn_t::get_elements()[index], k) < 1.e-6);

  return index;
}

template <class base_cluster_type>
void set_symmetry_matrices<base_cluster_type>::print_on_shell() {
  if (true) {
    FUNC_LIB::function<std::pair<int, int>, dmn_2<dmn_2<k_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>&
        symmetry_matrix = k_cluster_type::get_symmetry_matrix();

    for (int i = 0; i < k_dmn_t::dmn_size(); ++i) {
      for (int j = 0; j < b_dmn_t::dmn_size(); ++j) {
        std::cout << "\t" << i << ", " << j << "\t|\t";

        for (int l = 0; l < sym_super_cell_dmn_t::dmn_size(); ++l)
          std::cout << "\t" << symmetry_matrix(i, j, l).first << ", "
                    << symmetry_matrix(i, j, l).second;

        std::cout << "\n";
      }
    }
    std::cout << "\n";
  }

  if (true) {
    FUNC_LIB::function<std::pair<int, int>, dmn_2<dmn_2<r_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>&
        symmetry_matrix = r_cluster_type::get_symmetry_matrix();

    for (int i = 0; i < r_dmn_t::dmn_size(); ++i) {
      for (int j = 0; j < b_dmn_t::dmn_size(); ++j) {
        std::cout << "\t" << i << ", " << j << "\t|\t";

        for (int l = 0; l < sym_super_cell_dmn_t::dmn_size(); ++l)
          std::cout << "\t" << symmetry_matrix(i, j, l).first << ", "
                    << symmetry_matrix(i, j, l).second;

        std::cout << "\n";
      }
    }
    std::cout << "\n";
  }

  if (true) {
    FUNC_LIB::function<std::pair<int, int>, dmn_2<dmn_2<k_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>&
        k_symmetry_matrix = k_cluster_type::get_symmetry_matrix();
    FUNC_LIB::function<std::pair<int, int>, dmn_2<dmn_2<r_dmn_t, b_dmn_t>, sym_super_cell_dmn_t>>&
        r_symmetry_matrix = r_cluster_type::get_symmetry_matrix();

    for (int i = 0; i < r_dmn_t::dmn_size(); ++i) {
      for (int j = 0; j < b_dmn_t::dmn_size(); ++j) {
        for (int l = 0; l < sym_super_cell_dmn_t::dmn_size(); ++l) {
          if (r_symmetry_matrix(i, j, l).first == -1 or r_symmetry_matrix(i, j, l).second == -1) {
            std::cout << "\t r_ind : " << i << "\t" << j << "\n\n";

            sym_super_cell_dmn_t::get_elements()[l].to_JSON(std::cout);

            throw std::logic_error(__FUNCTION__);
          }
        }
      }
    }

    for (int i = 0; i < r_dmn_t::dmn_size(); ++i) {
      for (int j = 0; j < b_dmn_t::dmn_size(); ++j) {
        for (int l = 0; l < sym_super_cell_dmn_t::dmn_size(); ++l) {
          if (k_symmetry_matrix(i, j, l).first == -1 or k_symmetry_matrix(i, j, l).second == -1) {
            std::cout << "\t k_ind : " << i << "\t" << j << "\n\n";

            sym_super_cell_dmn_t::get_elements()[l].to_JSON(std::cout);

            throw std::logic_error(__FUNCTION__);
          }
        }
      }
    }
  }
}

#endif  // PHYS_LIBRARY_DOMAINS_CLUSTER_SYMMETRIZATION_ALGORITHMS_SET_SYMMETRY_MATRICES_H
