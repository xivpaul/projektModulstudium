#include "CSV.hpp"

void CSV::setMetadata(std::string path) {

  metadata.clear();

  // Create path
  metadata.push_back(path);

  // Create format
  std::string format;
  format = path.substr(path.find_last_of(".") + 1);
  metadata.push_back(format);

  // Create time
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
  std::string timestamp(buffer);
  metadata.push_back(timestamp);
}

void CSV::read(std::string path) {
  nRows = 0;
  // Daten zuruecksetzen:
  columns.clear();
  // Read CSV file
  std::ifstream csv_file(path);

  std::string line;
  if (csv_file.is_open()) {
    while (csv_file) {
      csv_file >> line;

      std::stringstream ss(line);
      std::vector<Value> parsed;

      for (std::string token; std::getline(ss, token, ',');) {
        // Hier muss noch ueberarbeitet werden. Manchmal werden noch "
        // mitgenommen, was zu Fehlverhalten fuehrt
        if (token.size() >= 2 && token[0] == '"') {
          if (token[token.size() - 1] == '"') {
            token = token.substr(1, token.size() - 2);
          } else {
            token = token.substr(1, token.size());
          }
          parsed.push_back(Value(token));
        } else {
          parsed.push_back(Value(std::stod(token)));
        }
      }
      // Hier muss noch ueberarbeitet werden
      if (columns.size() == 0) {
        bool allStrings = true;
        for (Value cValue : parsed) {
          if (cValue.type != STRING) {
            allStrings = false;
            break;
          }
        }

        // Create columns and Column
        for (Value cValue : parsed) {
          if (allStrings) {
            columns.push_back(Column(cValue.stringValue));
          } else {
            Column column;
            column.values.push_back(cValue);
            columns.push_back(column);
            nRows++;
          }
        }

        continue;
      }

      int nrColumn = 0;
      for (Value cValue : parsed) {
        columns[nrColumn++].values.push_back(cValue);
      }
      nRows++;
    }
  }
  if (idx) {
    // Indizierung der Messpunkte hinzufuegen:
    auto ColumnName = Value("Index Messpunkte");
    Column column(ColumnName.stringValue);
    for (int i = 1; i <= nRows; i++) {
      auto messpt = Value(i * 1.0);
      column.values.push_back(messpt);
    }
    // columns.push_back(column);
    columns.insert(columns.begin(), column);
  }
}

void CSV::print() {
  std::cout << "| ";
  for (Column column : columns) {
    std::cout << column.name << " | ";
  }
  std::cout << std::endl;

  for (int row; row < nRows; row++) {
    std::cout << "  ";
    for (Column column : columns) {
      if (column.values[row].type == NUMBER) {
        std::cout << column.values[row].numberValue;
      } else {
        std::cout << column.values[row].stringValue;
      }
      std::cout << " | ";
    }
    std::cout << std::endl;
  }
}
void CSV::buildAnalysisMatrix() {
  AnalysisMatrix.clear();
  std::vector<std::string> spalte;

  int anzahl_spalten = columns.size();
  for (int i = 0; i < anzahl_spalten; i++) {
    for (int j = 0; j < ColumnCriteria.size(); j++) {
      spalte.push_back("nan");
    }
    AnalysisMatrix.push_back(spalte);
  }
  // Minimum:
  for (int i = 0; i < anzahl_spalten - idx; i++) {
    AnalysisMatrix[i][0] = std::to_string(columns[i + idx].minValue());
  }
  // Maximum:
  for (int i = 0; i < anzahl_spalten - idx; i++) {
    AnalysisMatrix[i][1] = std::to_string(columns[i + idx].maxValue());
  }
  // Mittelwert:
  for (int i = 0; i < anzahl_spalten - idx; i++) {
    AnalysisMatrix[i][2] = std::to_string(columns[i + idx].mean());
  }
  // Summe:
  for (int i = 0; i < anzahl_spalten - idx; i++) {
    AnalysisMatrix[i][3] = std::to_string(columns[i + idx].sum());
  }
  // Standardabweichung:
  for (int i = 0; i < anzahl_spalten - idx; i++) {
    AnalysisMatrix[i][4] = std::to_string(columns[i + idx].standardDeviation());
  }
}