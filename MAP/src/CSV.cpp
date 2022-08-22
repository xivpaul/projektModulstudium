#include "CSV.hpp"

void CSV::setMetadata(std::string path) {

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
  // Daten zurücksetzen:
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
        // Hier muss noch überarbeitet werden. Manchmal werden noch "
        // mitgenommen, was zu Fehlverhalten führt
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
      // Hier muss noch überarbeitet werden
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