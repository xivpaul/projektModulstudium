#include "CSV.hpp"

CSV::CSV(){};
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
void CSV::creatTransCSV(std::string FileWithPath) {
  std::ofstream outputfile(FileWithPath);
  for (int j = (0 + int(idx)); j < columns.size(); j++) { // neu
    outputfile << "\"" << columns[j].name << "\"";        // neu
    if (j < (columns.size() - 1)) {
      outputfile << ",";  // neu
    } else {              // neu
      outputfile << "\n"; // neu
    }                     // neu
  }
  // outputfile << "\n"; //alt
  for (int i = 0; i < columns[0].values.size(); i++) {
    for (int j = (0 + int(idx)); j < columns.size(); j++) {
      if (columns[j].values[i].type == NUMBER) {
        outputfile << columns[j].values[i].numberValue;
      } else {
        outputfile << "\"" << columns[j].values[i].stringValue << "\"";
      }
      if (j < (columns.size() - 1)) {
        outputfile << ",";  // neu
      } else {              // neu
        outputfile << "\n"; // neu
      }                     // neu
    }
  }
  outputfile.close();
};

void CSV::createMetadata(std::string FileWithPath) {
  // CSV csv;
  setMetadata(FileWithPath);
  std::ofstream outfile(FileWithPath + "meta");
  outfile << "Pfad: " + metadata[0] + "<br>";
  outfile << "Format: " + metadata[1] + "<br>";
  outfile << "Datum: " + metadata[2];
  outfile.close();
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

void CSV::transformColumnValues(std::string FileWithPath) {

  if (TransformOperation == "+") {

    for (int i = 0; i < columns[ColumnToTransform].values.size(); i++) {
      columns[ColumnToTransform].values[i].numberValue += TransformValue;
    }
  }
  if (TransformOperation == "-") {
    for (int i = 0; i < columns[ColumnToTransform].values.size(); i++) {
      columns[ColumnToTransform].values[i].numberValue -= TransformValue;
    }
  }
  if (TransformOperation == "/") {
    for (int i = 0; i < columns[ColumnToTransform].values.size(); i++) {
      columns[ColumnToTransform].values[i].numberValue /= TransformValue;
    }
  }
  if (TransformOperation == "*") {
    for (int i = 0; i < columns[ColumnToTransform].values.size(); i++) {
      columns[ColumnToTransform].values[i].numberValue *= TransformValue;
    }
  }
  CSV::creatTransCSV(FileWithPath);
}

std::string CSV::createDropDownString_Column() {
  std::string DD_String_Column =
      "<option value=\"0\">Keine Datei ausgewählt</option>";
  if (columns.size() > 0) {
    DD_String_Column = "<option value=" + std::to_string(ColumnToTransform) +
                       ">" + columns[ColumnToTransform].name + "</option>";
    int i = 0;
    for (Column column : columns) {
      if (i == 0) {
        i++;
        continue;
      }
      if (column.name != columns[ColumnToTransform].name) {
        DD_String_Column += ("<option value=" + std::to_string(i) + ">" +
                             column.name + "</option>");
      }
      i++;
    }
  }

  return DD_String_Column;
}

std::string CSV::createDropDownString_Operation() {
  std::vector<std::string> operations = {"+", "-", "*", "/"};
  std::string DD_String_Operation = "<option value=" + TransformOperation +
                                    ">" + TransformOperation + "</option>";

  for (auto oper : operations) {

    if (oper != TransformOperation) {
      DD_String_Operation +=
          ("<option value=" + oper + ">" + oper + "</option>");
    }
  }

  return DD_String_Operation;
}

std::string CSV::createInputValueString() {

  return "<input type=\"text\" id=\"id_transformvalue\" "
         "name=\"transformvalue\" value=" +
         std::to_string(TransformValue) + ">";
}

std::string CSV::createDropDownString_Files(std::string loaded_file,
                                            std::string DB_DIR) {
  std::vector<std::string> data_vector;

  // Vorhandene Daten unter Verzeichnis "DB_DIR" suchen:
  const std::filesystem::path database_path{DB_DIR};
  for (auto const &dir_entry :
       std::filesystem::directory_iterator{database_path}) {
    std::string filename = dir_entry.path().filename().string();

    // die Dateinamen werden im Stringvektor gepspeichert, wenn Sie nicht der
    // gerade geladenen Datei entsprechen und auf "csv" enden:
    if (filename != loaded_file and
        filename.substr(filename.length() - 3, filename.length()) == "csv") {
      data_vector.push_back(filename);
    }
  }

  // String definieren fuer Eintraege im Dropdownmenue:
  std::string OPTIONSLISTE;

  // wenn eine Datei geladen ist (Endung auf "csv"), dann soll der erste
  // Eintrag im Auswahlmenue dieser Datei entsprechen:
  if (loaded_file.substr(loaded_file.length() - 3, loaded_file.length()) ==
      "csv") {
    OPTIONSLISTE =
        "<option value=" + loaded_file + ">" + loaded_file + "</option>";
  }
  // Erzeugung des Dropdownmenues zur Auswahl der vorhandenden Messdateien
  // fuer HTML:
  for (std::string file : data_vector) {
    OPTIONSLISTE += ("<option value=" + file + ">" + file + "</option>");
  }
  return OPTIONSLISTE;
}

void CSV::downloadFileFromServer(std::string path_webroot,
                                 std::string path_databank,
                                 std::string filename) {
  // copy csv to webroot to offer client download of file
  try {
    try {
      std::filesystem::remove(path_webroot + "/download_folder/download.csv");
    } catch (...) {
      std::cout << "File not available!" << std::endl;
    }
    std::filesystem::copy(path_databank + filename,
                          path_webroot + "/download_folder/download.csv",
                          std::filesystem::copy_options::overwrite_existing);
  } catch (std::filesystem::filesystem_error &e) {
    std::cout << e.what() << '\n';
  }
}