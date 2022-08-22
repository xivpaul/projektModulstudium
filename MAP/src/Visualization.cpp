#include "Visualization.hpp"

Visualization::Visualization() {}

void Visualization::save(std::string path) {}

void Visualization::compute(std::string DB_DIR_Input, std::string chosen_file,
                            int y_column) {
  /* todo: actually, instead of fixed values for x and y the data should be
   * loaded from the CSV file */
  CSV csv;

  csv.read(DB_DIR_Input + chosen_file);

  xColumn = csv.columns[1].toString();
  yColumn = csv.columns[y_column].toString();
}