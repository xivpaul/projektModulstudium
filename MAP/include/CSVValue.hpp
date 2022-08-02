#pragma once

#include <string>

enum CSVType {
    STRING = 0,
    NUMBER = 1
};

class CSVValue {
public:
    CSVType type;

    double numberValue;
    std::string stringValue;

    CSVValue(double value);
    CSVValue(std::string value);

    std::string toString();
};