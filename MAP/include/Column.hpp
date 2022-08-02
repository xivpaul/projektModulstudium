#pragma once

#include <string>
#include <vector>

#include "CSVValue.hpp"

class Column {
public:
    std::string name;
    std::vector<CSVValue> values;

    bool isType(CSVType type);

    double sum();
    double mean();

    std::string toString();

    Column();
    Column(std::string name);
};