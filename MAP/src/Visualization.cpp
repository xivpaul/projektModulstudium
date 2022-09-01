#include "Visualization.hpp"

Visualization::Visualization() {}

void Visualization::save(std::string path) {}

void Visualization::compute(CSV *csv, std::string DB_DIR_Input,
                            std::string chosen_file, int chosen_columns[2]) {
  /* todo: actually, instead of fixed values for x and y the data should be
   * loaded from the CSV file */

  std::cout << csv->columns[1].values[0].numberValue << std::endl;
  xColumn = csv->columns[chosen_columns[0]].toString();
  yColumn = csv->columns[chosen_columns[1]].toString();
}

std::string Visualization::setXColumnOptions(CSV *csv, std::string loaded_file,
                                             int chosen_columns[2]) {
  // CSV csv;
  std::string httpColumnSetString;
  std::string X_SPALTENOPTIONEN;

  int i = 0;
  X_SPALTENOPTIONEN = "<option value=" + std::to_string(chosen_columns[0]) +
                      ">" + csv->columns[chosen_columns[0]].name + "</option>";
  for (Column column : csv->columns) {
    if (column.name != csv->columns[chosen_columns[0]].name) {
      X_SPALTENOPTIONEN += "<option value=" + std::to_string(i) + ">" +
                           column.name + "</option>";
    }
    i++;
  }
  return X_SPALTENOPTIONEN;
}

std::string Visualization::setYColumnOptions(CSV *csv, std::string loaded_file,
                                             int chosen_columns[2]) {
  // CSV csv;
  std::string httpColumnSetString;
  std::string Y_SPALTENOPTIONEN;

  int i = 0;
  Y_SPALTENOPTIONEN = "<option value=" + std::to_string(chosen_columns[0]) +
                      ">" + csv->columns[chosen_columns[1]].name + "</option>";
  for (Column column : csv->columns) {
    if (column.name != csv->columns[chosen_columns[1]].name) {
      Y_SPALTENOPTIONEN += "<option value=" + std::to_string(i) + ">" +
                           column.name + "</option>";
    }
    i++;
  }
  return Y_SPALTENOPTIONEN;
}
std::string Visualization::createAnalysisTableString(CSV *csv) {
  int anzahl_spalten = csv->columns.size();
  std::string HTMLTableString = "<tr><th></th>";

  // Spaltenueberschriften erzeugen:
  for (int i = csv->idx; i < anzahl_spalten;
       i++) { // Indizierung der Messdaten nicht in Tabelle (erste Spalte),
              // deshalb Start bei 1
    HTMLTableString += "<th>" + (csv->columns[i].name + "</th>");
  }
  HTMLTableString += "</tr>";
  //"Zeilenueberschrift" erzeugen
  for (int i = 0; i < csv->ColumnCriteria.size(); i++) {
    HTMLTableString += "<tr><td>" + csv->ColumnCriteria[i] + "</td>";
    // Werte befuellen:
    for (int j = 0; j < anzahl_spalten - csv->idx;
         j++) { // Indizierung der Messdaten nicht in Analyse (erste Spalte),
                // deshalb Start bei 1
      HTMLTableString += "<td>" + csv->AnalysisMatrix[j][i] +
                         "</td>"; // hier Werte fuer Mittelwert etc.
    }
    HTMLTableString += "</tr>";
  }
  return HTMLTableString;
}

std::string Visualization::showAnalysis(CSV *csv, std::string chosen_file) {
  // Create time
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), "vom %d.%m.%Y, %H:%M:%SUhr", timeinfo);
  std::string timestamp(buffer);
  std::string anzahl_messpunkte = std::to_string(csv->columns[0].values.size());
  std::string httpAnalysisReportString = "<head>\
      <title>Analysebericht anzeigen\
      </title>\
                                    <style>\
table, th, td {\
  border:1px solid black;\
}\
</style>\
</head>\
<body><div id='id_analysisreport'></div></body> \
<h1>Analysebericht der Messdatei \"" + chosen_file +
                                         "\"</h1>\
                                         <h3>" +
                                         timestamp + "</h3>\
                                         <br>\
<h3>Anzahl der Messpunkte: " + anzahl_messpunkte +
                                         "</h3>\
                                    <table>\
" + createAnalysisTableString(csv) + "\
</table>\
  <br>\
  <br>\
  <br>\
<form style=\"display: inline;\"action = \"http://localhost:8000\">\
<input type = \"submit\" value = \"Zurueck zur Startseite\"/>\
</form>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
<form style=\"display: inline;\">\
<a href = \"data:application/xml;charset=utf-8,your code here\" download=\"filename.txt\" >Als Textdatei speichern</a>\
</form>";

  return httpAnalysisReportString;
}

// Quelle Table erstellen: https://www.w3schools.com/html/html_tables.asp
