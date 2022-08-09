#include "Value.hpp"

Value::Value(const double value) {
  type = NUMBER;
  numberValue = value;
}

Value::Value(const std::string value) {
  type = STRING;
  stringValue = value;
}

std::string Value::toString() {
  if (type == STRING) {
    return stringValue;
  } else {
    return std::to_string(numberValue);
  }
}