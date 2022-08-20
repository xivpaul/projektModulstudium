#pragma once

#include <string>
#include <vector>

#include "Column.hpp"

class CSV {
public:
  std::vector<Column> columns;
  std::vector<std::string> metadata;

  void read(std::string path);
  void setMetadata(std::string path);
  Column getColumn(std::string name);
  std::vector<std::string> setMetadata();

  int nRows = 0;

  void print();
};