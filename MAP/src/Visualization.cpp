#include "Visualization.hpp"

Visualization::Visualization() {}

void Visualization::save(std::string path) {}

void Visualization::compute(std::string DB_DIR_Input, std::string chosen_file,
                            int chosen_columns[2]) {
  /* todo: actually, instead of fixed values for x and y the data should be
   * loaded from the CSV file */
  CSV csv;

  csv.read(DB_DIR_Input + chosen_file);

  xColumn = csv.columns[chosen_columns[0]].toString();
  yColumn = csv.columns[chosen_columns[1]].toString();
}

std::string Visualization::setXColumnOptions(CSV csv, std::string loaded_file,
                                             int chosen_columns[2]) {
  // CSV csv;
  std::string httpColumnSetString;
  std::string X_SPALTENOPTIONEN;

  int i = 0;
  X_SPALTENOPTIONEN = "<option value=" + std::to_string(chosen_columns[0]) +
                      ">" + csv.columns[chosen_columns[0]].name + "</option>";
  for (Column column : csv.columns) {
    if (column.name != csv.columns[chosen_columns[0]].name) {
      X_SPALTENOPTIONEN += "<option value=" + std::to_string(i) + ">" +
                           column.name + "</option>";
    }
    i++;
  }
  return X_SPALTENOPTIONEN;
}

std::string Visualization::setYColumnOptions(CSV csv, std::string loaded_file,
                                             int chosen_columns[2]) {
  // CSV csv;
  std::string httpColumnSetString;
  std::string Y_SPALTENOPTIONEN;

  int i = 0;
  Y_SPALTENOPTIONEN = "<option value=" + std::to_string(chosen_columns[0]) +
                      ">" + csv.columns[chosen_columns[1]].name + "</option>";
  for (Column column : csv.columns) {
    if (column.name != csv.columns[chosen_columns[1]].name) {
      Y_SPALTENOPTIONEN += "<option value=" + std::to_string(i) + ">" +
                           column.name + "</option>";
    }
    i++;
  }
  return Y_SPALTENOPTIONEN;
}