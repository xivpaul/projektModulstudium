#include "Visualization.hpp"

Visualization::Visualization() {}

void Visualization::save(std::string path) {}

void Visualization::compute(std::string DB_DIR_Input) {
  /* todo: actually, instead of fixed values for x and y the data should be
   * loaded from the CSV file */
  CSV csv;

  csv.read(DB_DIR_Input + "data.csv");

  xColumn = csv.columns[1].toString();
  yColumn = csv.columns[2].toString();
}