#pragma once

#include "CSV.hpp"
#include "Column.hpp"
#include <string>
#include <vector>

class Visualization {
private:
  std::string createAnalysisTableString(CSV *csv); // Tabelle Analysebericht

public:
  Visualization();
  // Diagramm zeichnen
  std::string xColumn;
  std::string yColumn;
  std::string setXColumnOptions(CSV *csv, std::string loaded_file,
                                int chosen_columns[2]);
  std::string setYColumnOptions(CSV *csv, std::string loaded_file,
                                int chosen_columns[2]);
  void compute(CSV *csv, std::string DB_DIR_Input, std::string chosen_file,
               int chosen_columns[2]);
  // Analysebericht erstellen
  std::string createAnalysisReportPlot(CSV *csv);
  std::string showAnalysis(CSV *csv, std::string chosen_file);
  // Transformationshistorie
  std::string createTransformationHistoryTableString();
  std::vector<std::string> trans_ColumnHistory;
  std::vector<std::string> trans_OperationHistory;
  std::vector<std::string> trans_ValueHistory;
  int Transformationszaehler = 0;
  void clearHistory();
};
