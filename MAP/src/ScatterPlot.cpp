#include "ScatterPlot.hpp"
// #include <iostream>
#include <string>

ScatterPlot::ScatterPlot() {}

/**
 * @brief Die Methode gibt einen HTML String aus, um auf der Weboberfläche
 * die Messdaten zu visualisieren, sowie eine Achsauswahl der X- und Y-Achse
 * vornehmen zu können.
 *
 * @param DB_DIR_Input Datenbankpfad des Servers (entspricht DB_DIR)
 * @param chosen_file Name der ausgewaehlte Messdatei
 * @param chosen_columns gewaehlte Spalte aus Dropdownliste fuer die X- und
 * Y-Achse als integer -Liste
 * @param csv csv - Objekt, welches die Informationen der eingelesenen Messdaten
 * enthaelt
 * @return std::string std::string Es wird ein modifizierter HTML String
 * "httpColumnSetString" zurückgegeben.
 */
std::string ScatterPlot::plot(std::string DB_DIR_Input, std::string chosen_file,
                              CSV csv) {
  compute(DB_DIR_Input, chosen_file, chosen_columns);
  std::string checked_lineplot = "";
  std::string checked_scatterplot = "";

  if (plotstyle == "markers") {
    checked_scatterplot = "checked = checked";
  }
  if (plotstyle == "lines+markers") {
    checked_lineplot = "checked = checked";
  }

  auto SPALTENOPTIONEN_X = setXColumnOptions(csv, chosen_file, chosen_columns);
  auto SPALTENOPTIONEN_Y = setYColumnOptions(csv, chosen_file, chosen_columns);
  std::string visualizationHttp =
      "<head><title>Messdaten visualisieren</title><script src='plotly-2.12.1.min.js'></script> \
                                    </head><body><div id='myDiv'></div></body> \
                                    <script>var trace1 = { \
                                        x: " +
      xColumn + /* [1, 2, 3, 4]*/ ", \
                                        y: " +
      yColumn + /* [10, 15, 13, 17]*/ ", \
                                        mode: '" +
      plotstyle + "', \
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
      <form style=\"display: inline;\" action =\"/setColumn\" method=\"post\" enctype=\"multipart/form-data\">\
          <label for=\"Spaltenwahl\">X-Achse waehlen:</label>\
          <select name=\"spalte\" id=\"id_xspalte\">\
            " +
      SPALTENOPTIONEN_X + "\
          </select>\
          &nbsp;&nbsp;&nbsp;\
           <label for=\"Spaltenwahl\">Y-Achse waehlen:</label>\
          <select name=\"spalte\" id=\"id_yspalte\">\
            " +
      SPALTENOPTIONEN_Y + "\
          </select>\
          <input type=\"submit\" value=\"OK\" />\
        </form>\
      <form style=\"display: inline;float:right\" action =\"/plotstyle\" method=\"post\" enctype=\"multipart/form-data\">\
<input type=\"radio\" id=\"id_lineplot\" name=\"PlotStyle\" onclick=\"submit()\" value=\"lines+markers\" " +
      checked_lineplot + ">\
<label for=\"id_lineplot\">LinePlot</label>\
<input type=\"radio\" id=\"id_scatterplot \" name=\"PlotStyle\" onclick=\"submit()\" value=\"markers\" " +
      checked_scatterplot + ">\
<label for=\"id_scatterplot\">ScatterPlot</label>\
</form>\
<br>\
<br>\
<br>\
<form action = \"http://localhost:8000\">\
<input type = \"submit\" value = \"Zurueck zur Startseite\"/>\
</form>";
  return visualizationHttp;
}

// Quellen:
// Radiobutton vorbelegen:
// https://stackoverflow.com/questions/5592345/how-to-select-a-radio-button-by-default
// &nbsp; erzeugt Leerzeichen, Quelle:
// https://www.computerbild.de/artikel/cb-Tipps-Internet-HTML-Leerzeichen-einfuegen-Eine-kurze-uebersicht-31428597.html
// Radiobuttons erzeugen, Quelle:
// https://www.w3schools.com/tags/tryit.asp?filename=tryhtml5_input_type_radio
// Zwei <form> in der selben Zeile:
// https://stackoverflow.com/questions/18587588/keep-two-html-forms-on-the-same-line-in-a-paragraph-tag