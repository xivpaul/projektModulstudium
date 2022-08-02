#pragma once

#include <string>
#include <vector>

#include "Column.hpp"

class CSV {
public:
    void read(std::string path);
    std::vector<Column> columns;
    int nRows = 0;

    void print();
};