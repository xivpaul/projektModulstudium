#pragma once

#include <string>
#include <vector>

#include "Value.hpp"

class Column {
public:
  std::string name;
  std::vector<Value> values;

  bool isType(Type type);

  double sum();
  double mean();

  std::string toString();

  Column();
  Column(std::string name);
};