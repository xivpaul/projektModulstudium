#include "ScatterPlot.hpp"
// #include <iostream>
#include <string>

ScatterPlot::ScatterPlot() {}

/**
 * @brief Die Methode gibt einen HTML String aus, um auf der Weboberflaeche
 * die Messdaten zu visualisieren, sowie eine Achsauswahl der X- und Y-Achse
 * vornehmen zu koennen.
 *
 * @param DB_DIR_Input Datenbankpfad des Servers (entspricht DB_DIR)
 * @param chosen_file Name der ausgewaehlte Messdatei
 * @param chosen_columns gewaehlte Spalte aus Dropdownliste fuer die X- und
 * Y-Achse als integer -Liste
 * @param csv csv - Objekt, welches die Informationen der eingelesenen Messdaten
 * enthaelt
 * @return std::string std::string Es wird ein modifizierter HTML String
 * "httpColumnSetString" zurueckgegeben.
 */
std::string ScatterPlot::plot(std::string DB_DIR_Input, std::string chosen_file,
                              int chosen_columns[2], CSV csv) {
  compute(DB_DIR_Input, chosen_file, chosen_columns);

  // Anmerkung:
  // Scatterplot: mode: 'lines+markers'
  // Lineplot: mode: 'lines+markers'

  auto SPALTENOPTIONEN_X = setXColumnOptions(csv, chosen_file, chosen_columns);
  auto SPALTENOPTIONEN_Y = setYColumnOptions(csv, chosen_file, chosen_columns);
  std::string visualizationHttp =
      "<head><script src='plotly-2.12.1.min.js'></script> \
                                    </head><body><div id='myDiv'></div></body> \
                                    <script>var trace1 = { \
                                        x: " +
      xColumn + /* [1, 2, 3, 4]*/ ", \
                                        y: " +
      yColumn + /* [10, 15, 13, 17]*/ ", \
                                        mode: 'markers', \
                                            type: 'scatter' \
                                        }; \
                                        var data = [trace1]; \
                                        var layout = {\
                                          title: ' " +
      chosen_file + "', \
                                         xaxis: {\
                                          title: '" +
      csv.columns[chosen_columns[0]].name +

      "'},\
                                         yaxis: {\
                                          title: '" +
      csv.columns[chosen_columns[1]].name +

      "'}\
                                        }; \
                                        Plotly.newPlot('myDiv', data, layout); \
                                    </script>\
      <form action =\"/setColumn\" method=\"post\" enctype=\"multipart/form-data\">\
          <label for=\"Spaltenwahl\">X-Achse waehlen:</label>\
          <select name=\"spalte\" id=\"id_xspalte\">\
            " +
      SPALTENOPTIONEN_X + "\
          </select>\
           <label for=\"Spaltenwahl\">Y-Achse waehlen:</label>\
          <select name=\"spalte\" id=\"id_yspalte\">\
            " +
      SPALTENOPTIONEN_Y + "\
          </select>\
          <input type=\"submit\" value=\"OK\" />\
        </form>\
        <hr>\
        <form action = \"http://localhost:8000\">\
        <input type = \"submit\" value = \"Zurueck zur Startseite\"/>\
        </form>";
  return visualizationHttp;
}