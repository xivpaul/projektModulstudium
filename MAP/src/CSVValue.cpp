#include "CSVValue.hpp"

CSVValue::CSVValue(const double value) {
    type = NUMBER;
    numberValue = value;
}

CSVValue::CSVValue(const std::string value) {
    type = STRING;
    stringValue = value;
}

std::string CSVValue::toString() {
    if (type == STRING) {
        return stringValue;
    } else {
        return std::to_string(numberValue);
    }
}