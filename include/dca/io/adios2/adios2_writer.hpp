// Copyright (C) 2018 ETH Zurich
// Copyright (C) 2018 UT-Battelle, LLC
// All rights reserved.
//
// See LICENSE for terms of usage.
// See CITATION.md for citation guidelines, if DCA++ is used for scientific publications.
//
// Author: Norbert Podhorszki (pnorbert@ornl.gov)
//
// ADIOS2 writer.

#ifndef DCA_IO_ADIOS2_ADIOS2_WRITER_HPP
#define DCA_IO_ADIOS2_ADIOS2_WRITER_HPP

#include <complex>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "adios2.h"

#include "dca/function/domains.hpp"
#include "dca/io/buffer.hpp"
#include "dca/function/function.hpp"
#include "dca/linalg/matrix.hpp"
#include "dca/linalg/vector.hpp"

namespace dca {
namespace io {
// dca::io

class ADIOS2Writer {
public:
  typedef adios2::ADIOS file_type;

public:
  // In: verbose. If true, the writer outputs a short log whenever it is executed.
  ADIOS2Writer(const std::string& config = "", bool verbose = true);
  ~ADIOS2Writer();

  constexpr bool is_reader() {
    return false;
  }
  constexpr bool is_writer() {
    return true;
  }

  void open_file(const std::string& file_name_ref, bool overwrite = true);
  void close_file();

  void open_group(const std::string& new_path);
  void close_group();

  std::string get_path(const std::string& name = "");

  template <typename arbitrary_struct_t>
  static void to_file(const arbitrary_struct_t& arbitrary_struct, const std::string& file_name);

  template <typename Scalar>
  void execute(const std::string& name, Scalar value);

  template <typename Scalar>
  void execute(const std::string& name, const std::pair<Scalar, Scalar>& value);

  template <typename Scalar>
  void execute(const std::string& name, const std::vector<Scalar>& value);

  void execute(const std::string& name, const std::string& value);

  void execute(const std::string& name, const std::vector<std::string>& value);

  template <typename Scalar, std::size_t n>
  void execute(const std::string& name, const std::vector<std::array<Scalar, n>>& value);

  template <typename Scalar>
  void execute(const std::string& name, const std::vector<std::vector<Scalar>>& value);

  template <typename domain_type>
  void execute(const std::string& name, const func::dmn_0<domain_type>& dmn);

  template <typename Scalar, typename domain_type>
  void execute(const func::function<Scalar, domain_type>& f);

  template <typename Scalar, typename domain_type>
  void execute(const std::string& name, const func::function<Scalar, domain_type>& f);

  template <typename Scalar>
  void execute(const std::string& name, const dca::linalg::Vector<Scalar, dca::linalg::CPU>& A);

  template <typename Scalar>
  void execute(const std::string& name, const dca::linalg::Matrix<Scalar, dca::linalg::CPU>& A);

  template <typename Scalar>
  void execute(const dca::linalg::Matrix<Scalar, dca::linalg::CPU>& A) {
    execute(A.get_name(), A);
  }

  template <class T>
  void execute(const std::string& name, const std::unique_ptr<T>& obj);

  template <class T>
  void execute(const std::unique_ptr<T>& obj);

  void execute(const std::string& name, const io::Buffer& buffer) {
    return execute(name, static_cast<io::Buffer::Container>(buffer));
  }

  operator bool() const {
    return (file_ ? true : false);
    // return static_cast<bool>(file_);
  }

  void lock() {
    mutex_.lock();
  }

  void unlock() {
    mutex_.unlock();
  }

private:
  // bool fexists(const char* filename);

  // bool exists(const std::string& name) const;

  template <typename Scalar>
  void write(const std::string& name, const std::vector<size_t>& size, const Scalar* data);

  void write(const std::string& name, const std::string& data);

  template <typename Scalar>
  void addAttribute(const std::string& set, const std::string& name,
                    const std::vector<size_t>& size, const Scalar* data);

  void addAttribute(const std::string& set, const std::string& name, const std::string& value);

  adios2::ADIOS adios_;
  const bool verbose_;

  adios2::IO io_;
  std::string io_name_;
  std::string file_name_;
  adios2::Engine file_;

  std::vector<std::string> my_paths_;

  std::mutex mutex_;

  std::vector<size_t> size_check_;
};

template <typename arbitrary_struct_t>
void ADIOS2Writer::to_file(const arbitrary_struct_t& arbitrary_struct, const std::string& file_name) {
  ADIOS2Writer wr_obj;
  wr_obj.open_file(file_name);
  arbitrary_struct.read_write(wr_obj);
  wr_obj.close_file();
}

template <typename Scalar>
void ADIOS2Writer::execute(const std::string& name, Scalar value) {
  const std::string full_name = get_path(name);
  std::vector<size_t> dims{1};

  write<Scalar>(full_name, dims, &value);
}

template <typename Scalar>
void ADIOS2Writer::execute(const std::string& name, const std::pair<Scalar, Scalar>& value) {
  std::string full_name = get_path(name);
  std::vector<size_t> dims{2};

  write<Scalar>(full_name, dims, &value.first);
}

template <typename Scalar>
void ADIOS2Writer::execute(const std::string& name,
                           const std::vector<Scalar>& value)  //, H5File& file, std::string path)
{
  if (value.size() > 0) {
    std::string full_name = get_path(name);
    std::vector<size_t> dims{value.size()};
    write<Scalar>(full_name, dims, value.data());
  }
}

template <typename Scalar>
void ADIOS2Writer::execute(const std::string& name, const std::vector<std::vector<Scalar>>& value) {
  std::string full_name = get_path(name);

  throw(std::logic_error("ADIOS does not support vector of vectors. name = " + name + " (" +
                         __FUNCTION__ + ")"));
}

template <typename Scalar, std::size_t n>
void ADIOS2Writer::execute(const std::string& name, const std::vector<std::array<Scalar, n>>& value) {
  if (value.size() == 0)
    return;

  throw(std::logic_error("ADIOS does not support variable length arrays. name = " + name + " (" +
                         __FUNCTION__ + ")"));
}

template <typename domain_type>
void ADIOS2Writer::execute(const std::string& name, const func::dmn_0<domain_type>& dmn) {
  open_group(name);

  execute("name", dmn.get_name());
  execute("elements", dmn.get_elements());

  close_group();
}

template <typename Scalar, typename domain_type>
void ADIOS2Writer::execute(const func::function<Scalar, domain_type>& f) {
  if (f.size() == 0)
    return;

  if (verbose_)
    std::cout << "\t starts ADIOS2 writing function : " << f.get_name() << "\n";

  execute(f.get_name(), f);
}

template <typename Scalar, typename domain_type>
void ADIOS2Writer::execute(const std::string& name, const func::function<Scalar, domain_type>& f) {
  if (f.size() == 0)
    return;

  const std::string full_name = get_path(name);

  std::vector<size_t> dims;
  for (int l = 0; l < f.signature(); ++l)
    dims.push_back(f[l]);

  // be careful --> ADIOS2 is by default row-major, while the function-class is column-major !
  std::reverse(dims.begin(), dims.end());

  write<Scalar>(full_name, dims, f.values());

  std::reverse(dims.begin(), dims.end());
  addAttribute(full_name, "name", f.get_name());
  addAttribute<size_t>(full_name, "domain-sizes", std::vector<size_t>{dims.size()}, dims.data());
}

template <typename Scalar>
void ADIOS2Writer::execute(const std::string& name,
                           const dca::linalg::Vector<Scalar, dca::linalg::CPU>& V) {
  std::string full_name = get_path(name);
  write<Scalar>(full_name, std::vector<size_t>{V.size()}, V.ptr());
  addAttribute(full_name, "name", V.get_name());
}

template <typename Scalar>
void ADIOS2Writer::execute(const std::string& name,
                           const dca::linalg::Matrix<Scalar, dca::linalg::CPU>& A) {
  std::vector<size_t> dims{size_t(A.nrRows()), size_t(A.nrCols())};
  std::vector<Scalar> linearized(dims[0] * dims[1]);

  int linindex = 0;
  // Note: Matrices are row major, while ADIOS2 is column major
  for (int i = 0; i < A.nrRows(); ++i)
    for (int j = 0; j < A.nrCols(); ++j)
      linearized[linindex++] = A(i, j);

  std::string full_name = get_path(name);
  write<Scalar>(full_name, dims, linearized.data());

  addAttribute(full_name, "name", A.get_name());
}

template <class T>
void ADIOS2Writer::execute(const std::string& name, const std::unique_ptr<T>& obj) {
  if (obj)
    execute(name, *obj);
}

template <class T>
void ADIOS2Writer::execute(const std::unique_ptr<T>& obj) {
  if (obj)
    execute(*obj);
}

template <typename Scalar>
void ADIOS2Writer::write(const std::string& name, const std::vector<size_t>& size,
                         const Scalar* data) {
  size_t ndim = size.size();
  adios2::Variable<Scalar> v;
  if (ndim == 0) {
    v = io_.DefineVariable<Scalar>(name);
  }
  else {
    std::vector<size_t> start(ndim, 0);
    v = io_.DefineVariable<Scalar>(name, size, start, size);
  }
  file_.Put(v, data, adios2::Mode::Sync);
}

template <typename Scalar>
void ADIOS2Writer::addAttribute(const std::string& set, const std::string& name,
                                const std::vector<size_t>& size, const Scalar* data) {
  size_t ndim = size.size();
  if (ndim == 0) {
    io_.DefineAttribute(name, data, 1, set);
  }
  else if (ndim == 1) {
    io_.DefineAttribute(name, data, size[0], set);
  }
  else {
    throw(std::logic_error("ADIOS does not support multi-dimensional Attributes name = " + name +
                           " ndim = " + std::to_string(ndim) + "(" + __FUNCTION__ + ")"));
  }
}

}  // namespace io
}  // namespace dca

#endif  // DCA_IO_ADIOS2_ADIOS2_WRITER_HPP
