#pragma once

#include "CSV.hpp"
#include "Column.hpp"
#include <string>

class Visualization {
private:
public:
  Visualization();
  std::string xColumn;
  std::string yColumn;
  void save(std::string path);
  void compute(std::string DB_DIR_Input, std::string chosen_file, int y_column);
};
