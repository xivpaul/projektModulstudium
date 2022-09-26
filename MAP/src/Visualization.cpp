#include "Visualization.hpp"

Visualization::Visualization() {}

void Visualization::save(std::string path) {}

void Visualization::compute(CSV *csv, std::string DB_DIR_Input,
                            std::string chosen_file, int chosen_columns[2]) {
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
              // deshalb Start bei csv->idx, Standardeinstellung = 1)
    HTMLTableString += "<th>" + (csv->columns[i].name + "</th>");
  }
  HTMLTableString += "</tr>";
  //"Zeilenueberschrift" erzeugen
  for (int i = 0; i < csv->ColumnCriteria.size(); i++) {
    HTMLTableString += "<tr><td>" + csv->ColumnCriteria[i] + "</td>";
    // Werte befuellen:
    for (int j = 0; j < anzahl_spalten - csv->idx; j++) {
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
  std::string httpAnalysisReportString = "<!DOCTYPE html>\
<html lang=\"en\">\
 <meta charset=\"utf-8\" />\
  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\
  <link\
    href=\"https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&display=swap\"\
    rel=\"stylesheet\">\
  <link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\">\
<head>\
      <title>Analysebericht von " + chosen_file +
                                         "\
      </title>\
                                    <style>\
      #outer_frame {\
      margin-right: auto;\
      margin-left: auto;\
      width: 1000px;\
    }\
      #frame {\
      margin-bottom: 0px;\
      background: #ffffff;\
      border-radius: .5em;\
      padding: 2em;\
      border: 1px solid black;\
    }\
      #plot_frame {\
      background: #ffffff;\
      border-radius: .5em;\
      padding: 2em;\
      border: 1px solid black;\
    }\
table{\
  border:1px solid black;\
  width:900px;\
  font-size: 13px;\
}\
 th, td {\
  border:1px solid black;\
}\
body {\
color: black;\
}\
</style>\
</head>\
<body>\
<div id='outer_frame'>\
  <div id = 'frame'>\
    <h1 style=\"margin-top:0px;\"> Analysebericht der Messdatei \"" +
                                         chosen_file + "\"</h1><h3>" +
                                         timestamp + "</h3>\
    <h4>Anzahl der Messpunkte: " + anzahl_messpunkte +
                                         "</h4>\
  </div>\
  <div id='frame'>\
    <h3 style=\"margin-top:-5px;\"> Transformationshistorie</h3>\
    <table >\
              <tr>\
                <th WIDTH=\"10%\">Nr.</th>\
                <th WIDTH=\"30%\">Spaltenname</th>\
                <th WIDTH=\"30%\">Operation</th>\
                <th WIDTH=\"30%\">Eingabewert</th>\
              </tr>\
     " + createTransformationHistoryTableString() +
                                         "</table>\
  </div>\
  <div id ='frame'>\
    <h3 style=\"margin-top:-5px;\"> Übersicht zur Messdatenbewertung</h3>\
    <table> " + createAnalysisTableString(csv) +
                                         " </table >\
  </div>\
  <div id='frame'><h3 style=\"margin-top:-5px;margin-bottom:0px;\"> Visualisierung</h3>" +
                                         createAnalysisReportPlot(csv) +
                                         "</div>\
                                         <br>\
<form style=\"display: inline;\"action = \"http://localhost:8000\">\
<input id='backHome' style=\"color: white;\" type = \"submit\" value = \"Zurück zur Startseite\"/>\
</form>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
<button onclick=\"printFunction();\" style=\"color: white;\">\
Drucken\
      </button><script> function printFunction() {\
                                         window.print();\
}\
</script>\
</div>\
<script type =\"module\" src=\"https://unpkg.com/ionicons@5.5.2/dist/ionicons/ionicons.esm.js\"></script>\
</ body>";
  return httpAnalysisReportString;
}

void Visualization::clearHistory() {
  trans_ColumnHistory.clear();
  trans_ValueHistory.clear();
  trans_OperationHistory.clear();
  Transformationszaehler = 0;
}

std::string Visualization::createTransformationHistoryTableString() {
  std::vector<std::string> trans_Number;

  for (int i = 1; i <= Transformationszaehler; i++) {
    trans_Number.push_back(std::to_string(i));
  }

  std::string HTMLTableString;

  // Werte befuellen:
  for (int j = 0; j < trans_ColumnHistory.size(); j++) {
    HTMLTableString += "<tr><td WIDTH=\"10%\"> " + trans_Number[j] +
                       "</td>"; // Nummerierung der Operation hinzufuegen
    HTMLTableString += "<td width=\"30%\">" + trans_ColumnHistory[j] +
                       "</td>"; // Spaltenname hinzufuegen
    HTMLTableString += "<td WIDTH=\"30%\">" + trans_OperationHistory[j] +
                       "</td>"; // Operation hinzufuegen
    HTMLTableString += "<td WIDTH=\"30%\">" + trans_ValueHistory[j] +
                       "</td></tr>"; // eingegebenen Wert hinzufuegen
  }

  return HTMLTableString;
}

std::string Visualization::createAnalysisReportPlot(CSV *csv) {

  std::string xColumn = csv->columns[0].toString();

  std::string traces;
  std::string data = "var data = [";
  for (int i = 1; i < csv->columns.size(); i++) {
    if (not csv->columns[i].isType(NUMBER)) {
      continue;
    } else {
      if (i > 1 && i < csv->columns.size()) {
        data += ",";
      }
    }
    std::string yColumn = csv->columns[i].toString();
    traces += "var " + csv->columns[i].name + " = {x:" + xColumn +
              ",y:" + yColumn + ",\
  mode: 'lines',\
  name: '" + csv->columns[i].name +
              "'\
};";
    data += csv->columns[i].name;
  }

  data += "];";
  std::string visualizationHttp =
      "<div id ='myDiv' style=\"margin-top: 0px;\"><script src='plotly-2.12.1.min.js'></script> \
                                    <script>" +
      traces + data + "\
                                        var layout = {\
                                        width: 900,\
                                        showlegend: true,\
                                        autosize: false, \
                                         xaxis: {\
                                          title: 'Index Messpunkte'},\
                                         yaxis: {\
                                          title: 'Messwert'}\
                                        }; \
                                        Plotly.newPlot('myDiv', data,layout); \
                                    </script></div>";

  return visualizationHttp;
}

// Quelle Table erstellen: https://www.w3schools.com/html/html_tables.asp

// Quelle für Print/speichern https://www.csestack.org/code-print-save-html-pdf/
