#pragma once

#include "Value.hpp"
#include <algorithm> // verwendet fuer std::min_element
#include <iostream>
#include <iterator> // verwendet fuer std::begin, std::end
#include <string>
#include <vector>

class Column {
public:
  std::string name;
  std::vector<Value> values;

  bool isType(Type type);

  double sum();
  double mean();
  double minValue();
  double maxValue();
  double standardDeviation();

  std::string toString();

  Column();
  Column(std::string name);
};