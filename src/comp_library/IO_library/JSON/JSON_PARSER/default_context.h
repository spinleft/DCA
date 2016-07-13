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

#ifndef COMP_LIBRARY_IO_LIBRARY_JSON_JSON_PARSER_DEFAULT_CONTEXT_H
#define COMP_LIBRARY_IO_LIBRARY_JSON_JSON_PARSER_DEFAULT_CONTEXT_H

#include <iostream>
#include <string>
#include <vector>

#include "comp_library/IO_library/JSON/JSON_PARSER/parse_buffer.h"
#include "comp_library/IO_library/JSON/JSON_PARSER/what_ever.h"
#include "comp_library/IO_library/JSON/JSON_PARSER/JSON_operators.h"

namespace IO {
namespace JSONPARSER {
class JSON_context {
public:
  JSON_context();

  void begin_array();
  void begin_object();
  void begin_numeric_array(std::string filename, size_t charNum);

  void end_array();
  void end_object();
  void end_numeric_array(size_t charNum);

  void Integer(ParseBuffer& s);

  void Float(ParseBuffer& s);

  void Null();

  void True();
  void False();

  void String(ParseBuffer& s);

  void Key(const std::wstring& s);

private:
  Whatever& currentObject();

  //* Object and Array Referenced objects are created when needed on LHS of assignment! */
  Whatever& referencedObject();

  template <JSON_whatever_type type_id>
  void beginObjectOrArray();

  template <int TypeId>
  void endObjectOrArray();

  std::string CurrentContext();

  template <typename T>
  void set(const T& v);

public:
  Whatever result;
  std::vector<Whatever*> stack;
  std::wstring key;
  bool trace;
};

JSON_context::JSON_context() : result(), stack(1, &result), key(L""), trace(false) {}

void JSON_context::begin_object() {
  beginObjectOrArray<WHATEVER_MAP>();
}

void JSON_context::end_object() {
  endObjectOrArray<WHATEVER_MAP>();
}

void JSON_context::begin_numeric_array(std::string filename, size_t charNum) {
  beginObjectOrArray<WHATEVER_MAT>();
  currentObject().filename = filename;
  currentObject().startPos = charNum;
}

void JSON_context::end_numeric_array(size_t charNum) {
  currentObject().endPos = charNum;
  endObjectOrArray<WHATEVER_MAT>();
}

void JSON_context::begin_array() {
  beginObjectOrArray<WHATEVER_VECTOR>();
}

void JSON_context::end_array() {
  endObjectOrArray<WHATEVER_VECTOR>();
}

Whatever& JSON_context::currentObject() {
  return *stack.back();
}

//* Object and Array Referenced objects are created when needed on LHS of assignment! */
Whatever& JSON_context::referencedObject() {
  Whatever& current(currentObject());

  switch (current.type) {
    case WHATEVER_MAP: {
      Whatever& result(current.whateverMap[key]);
      result.myKey = key;
      result.parent = &current;

      if (trace)
        std::cout << "   referencedMap => '" << result.name() << "' !\n";

      return result;
    }

    case WHATEVER_VECTOR: {
      size_t idx = current.size();
      current.push_back();
      Whatever& result(current.back());
      result.myIndex = idx;
      result.parent = &current;

      if (trace)
        std::cout << "   referencedVector => '" << result.name() << "' !\n";

      return result;
    }

    default:
      if (trace)
        std::cout << "   referencedObject => '" << current.name() << "' !\n";

      return current;
  }
}

template <JSON_whatever_type TypeId>
void JSON_context::beginObjectOrArray() {
  Whatever& refObj = referencedObject();  // Generally creates the object or array
                                          // (except for the first time when refObject == result )
  refObj.type = TypeId;                   // Sets the type
  if (&refObj != &result)      // result is already on the stack, so we dont' need to push it on.
    stack.push_back(&refObj);  // Make the referenced object the current object

  if (trace)
    std::cout << " Set the type of " << refObj.name() << " to " << name(TypeId)
              << " and make it the current object\n";
}

template <int TypeId>
void JSON_context::endObjectOrArray() {
  if (trace)
    std::cout << "   JSON_context is ending object '" << currentObject().name()
              << "' by popping the object stack!\n";

  assert(currentObject().type == TypeId);

  stack.pop_back();

  if (trace) {
    if (stack.size() > 0) {
      std::cout << "   current object is now '" << currentObject().name() << "\n";
    }
    else
      std::cout << "   Parsing completed! \n ";
  }
}

std::string JSON_context::CurrentContext() {
  return referencedObject().name();
}

template <typename T>
void JSON_context::set(const T& v) {
  Whatever& refObj(referencedObject());
  refObj = v;
}

void JSON_context::Integer(ParseBuffer& s) {
  int i;
  s >> i;
  set(i);
}

void JSON_context::Float(ParseBuffer& s) {
  double d;
  s >> d;
  set(d);
}

void JSON_context::Null() {
  set(Whatever::null());
}

void JSON_context::True() {
  set(true);
}

void JSON_context::False() {
  set(false);
}

void JSON_context::String(ParseBuffer& s) {
  set(s.str());
}

void JSON_context::Key(const std::wstring& s) {
  key = s;
  if (trace)
    std::wcout << L"   key = '" << key << L"'\n";
}

}  // namespace JSONPARSER
}  // namespace IO

#endif  // COMP_LIBRARY_IO_LIBRARY_JSON_JSON_PARSER_DEFAULT_CONTEXT_H
