#pragma once

#include "Visualization.hpp"
#include <string>

class ScatterPlot : public Visualization {
private:
public:
  ScatterPlot();
  std::string plot(std::string DB_DIR_Input, std::string chosen_file,
                   int chosen_columns[2], CSV csv);
};