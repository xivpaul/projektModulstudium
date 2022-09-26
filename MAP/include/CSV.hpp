#pragma once

#include "Column.hpp"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class CSV {
private:
public:
  // Konstruktor
  CSV();
  bool idx = true; // erste Spalte enthaelt Nummerierung der Messpunkte ja
                   // (true) nein (false)
  std::vector<std::string> ColumnCriteria = {"Minimalwert", "Maximalwert",
                                             "Mittelwert", "Summe",
                                             "Standardabweichung"};
  std::vector<Column> columns;
  std::vector<std::string> metadata;
  // Datenhandling
  void read(std::string path);
  void createMetadata(std::string FileWithPath);
  void setMetadata(std::string path);
  Column getColumn(std::string name);
  void downloadFileFromServer(std::string path_webroot,
                              std::string path_databank, std::string filename);
  int nRows = 0;
  // Analyse
  void buildAnalysisMatrix();
  std::vector<std::vector<std::string>> AnalysisMatrix;
  // Transformation
  int ColumnToTransform = 1;
  std::string TransformOperation = "+";
  double TransformValue = 0.0;
  void transformColumnValues(std::string FileWithPath);
  void creatTransCSV(std::string FileWithPath);
  // HTML Strings fuer Anzeige
  std::string createDropDownString_Column();
  std::string createDropDownString_Operation();
  std::string createInputValueString();
  std::string createDropDownString_Files(std::string loaded_file,
                                         std::string DB_DIR);
};