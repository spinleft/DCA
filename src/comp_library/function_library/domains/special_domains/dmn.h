// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Peter Staar (taa@zurich.ibm.com)
//         Urs R. Haehner (haehneru@itp.phys.ethz.ch)
//
// This file implements a simple, but generic domain that can be used with the function class.
// It's templated on its size and the element type. If needed, elements can be assigned and stored.
//
// TODO: The size type should be std::size_t. However, since the underlying type of std::size_t is
//       compiler dependent, the name of the domain would be compiler dependent, too. This would
//       make writing tests a little harder.

#ifndef COMP_LIBRARY_FUNCTION_LIBRARY_DOMAINS_SPECIAL_DOMAINS_DMN_H
#define COMP_LIBRARY_FUNCTION_LIBRARY_DOMAINS_SPECIAL_DOMAINS_DMN_H

#include <stdexcept>
#include <string>
#include <vector>

#include "dca/util/type_utils.hpp"  // for dca::util::type_name

template <int size, class element_t = int>
class dmn {
public:
  static_assert(size > 0, "Size must pe positive.");

  using element_type = element_t;
  using this_type = dmn<size, element_t>;

  static int get_size() {
    return size;
  }
  static int dmn_size() {
    return get_size();
  }

  static std::string get_name() {
    return dca::util::type_name<this_type>();
  }

  static void set_elements(const std::vector<element_t>& elements) {
    if (elements.size() != size)
      throw std::logic_error(
          "The size of the passed elements doesn't match the size of the domain.");
    elements_ = elements;
  }
  static const std::vector<element_t>& get_elements() {
    if (elements_.size() != size)
      throw std::logic_error("Elements have not been set.");
    return elements_;
  }

private:
  static std::vector<element_t> elements_;
};

template <int size, class element_t>
std::vector<element_t> dmn<size, element_t>::elements_;

#endif  // COMP_LIBRARY_FUNCTION_LIBRARY_DOMAINS_SPECIAL_DOMAINS_DMN_H
