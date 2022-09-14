#pragma once

#include "CSV.hpp"
#include "Column.hpp"
#include <string>
#include <vector>

class Visualization {
private:
  std::string createAnalysisTableString(CSV *csv);
  const int maxHistoryEntries = 7;

public:
  Visualization();
  std::string xColumn;
  std::string yColumn;
  std::string createAnalysisReportPlot(CSV *csv);
  std::string
  setXColumnOptions(CSV *csv, std::string loaded_file,
                    int chosen_columns[2]); // diese Methoden dürfen nicht
                                            // private sein. Warum nicht?
  std::string setYColumnOptions(CSV *csv, std::string loaded_file,
                                int chosen_columns[2]);
  std::string showAnalysis(CSV *csv, std::string chosen_file);
  void save(std::string path);
  void compute(CSV *csv, std::string DB_DIR_Input, std::string chosen_file,
               int chosen_columns[2]);
  std::string createTransformationHistoryTableString();
  std::vector<std::string> trans_ColumnHistory;
  std::vector<std::string> trans_OperationHistory;
  std::vector<std::string> trans_ValueHistory;
  int Transformationszaehler = 0;
  void clearHistory();
};
