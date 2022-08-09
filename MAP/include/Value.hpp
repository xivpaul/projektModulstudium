#pragma once

#include <string>

enum Type { STRING = 0, NUMBER = 1 };

class Value {
public:
  Type type;

  double numberValue;
  std::string stringValue;

  Value(double value);
  Value(std::string value);

  std::string toString();
};