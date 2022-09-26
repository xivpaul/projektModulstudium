#include "Column.hpp"

#include <cmath>
/**
 * @brief Construct a new Column:: Column object
 *
 */
Column::Column() { name = "UNKNOWN"; };

Column::Column(std::string _name) { name = _name; }

bool Column::isType(Type type) {
  for (Value value : values) {
    if (value.type != type) {
      return false;
    }
  }
  return true;
}

double Column::sum() {
  if (!isType(NUMBER)) {
    return NAN;
  }

  double sum = 0.0;
  for (Value value : values) {
    sum += value.numberValue;
  }
  return sum;
}

double Column::mean() { return sum() / values.size(); }

std::string Column::toString() {
  std::string result = "[";

  bool first = true;

  for (Value value : values) {
    if (!first) {
      result = result + ",";
    }
    result = result + value.toString();
    first = false;
  }

  result = result + "]";

  return result;
}

double Column::minValue() {
  if (!isType(NUMBER)) {
    return NAN;
  }
  double minimum = values[0].numberValue * 1.0;
  // std::cout << minimum << std::endl;

  for (Value value : values) {
    if (value.numberValue < minimum) {
      minimum = value.numberValue;
    }
  }
  return minimum;
}

double Column::maxValue() {
  if (!isType(NUMBER)) {
    return NAN;
  }
  double maximum = values[0].numberValue * 1.0;
  ;
  for (Value value : values) {
    if (value.numberValue > maximum) {
      maximum = value.numberValue;
    }
  }
  return maximum;
}
double Column::standardDeviation() {
  if (!isType(NUMBER)) {
    return NAN;
  }
  double deviation = 0;
  for (Value value : values) {
    deviation += pow(value.numberValue - Column::mean(), 2);
  }
  return sqrt(deviation / values.size());
}
