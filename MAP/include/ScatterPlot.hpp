#pragma once

#include "Visualization.hpp"
#include <string>

class ScatterPlot : public Visualization {
private:
public:
  ScatterPlot();
  std::string plotstyle = "markers";
  int chosen_columns[2] = {0, 1};
  std::string plot(std::string DB_DIR_Input, std::string chosen_file, CSV csv);
};