#pragma once
#include "Visualization.hpp"
#include <string>

class Plot : public Visualization {
private:
  void calcFilteredData(CSV *csv, int filtersize, std::string *result_data,
                        std::string *result_time);

public:
  Plot();
  std::string plotstyle = "markers";
  int chosen_columns[2] = {0, 1};
  std::string plot(CSV *csv, std::string DB_DIR_Input, std::string chosen_file,
                   std::string infoAlert);
  int filtersize = 9; // initialwert
  std::string smoothed_data = "";
};