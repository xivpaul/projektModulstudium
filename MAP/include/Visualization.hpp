#pragma once

#include "CSV.hpp"
#include "Column.hpp"
#include <string>
#include <vector>

class Visualization {
private:
  std::string createAnalysisTableString(CSV csv);

public:
  Visualization();
  std::string xColumn;
  std::string yColumn;
  std::string
  setXColumnOptions(CSV csv, std::string loaded_file,
                    int chosen_columns[2]); // diese Methoden dürfen nicht
                                            // private sein. Warum nicht?
  std::string setYColumnOptions(CSV csv, std::string loaded_file,
                                int chosen_columns[2]);
  std::string showAnalysis(CSV csv, std::string chosen_file);
  void save(std::string path);
  void compute(std::string DB_DIR_Input, std::string chosen_file,
               int chosen_columns[2]);
};
