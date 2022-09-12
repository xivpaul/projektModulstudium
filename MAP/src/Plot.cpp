#include "Plot.hpp"
// Konstruktor:
Plot::Plot() {}

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
std::string Plot::plot(CSV *csv, std::string DB_DIR_Input,
                       std::string chosen_file) {
  compute(csv, DB_DIR_Input, chosen_file, chosen_columns);
  auto SPALTENOPTIONEN_X = setXColumnOptions(csv, chosen_file, chosen_columns);
  auto SPALTENOPTIONEN_Y = setYColumnOptions(csv, chosen_file, chosen_columns);

  std::string checked_lineplot = "";
  std::string checked_scatterplot = "";
  std::string checked_histogram = "";
  std::string YAxis = "";
  std::string style;
  std::string DropdownMenue_Y = "";
  std::string title_XAxis = csv->columns[chosen_columns[0]].name;
  std::string title_YAxis = "";
  if (plotstyle == "markers") {
    checked_scatterplot = "checked = checked";
  }
  if (plotstyle == "lines+markers") {
    checked_lineplot = "checked = checked";
  }
  if (plotstyle == "histogram") {
    checked_histogram = "checked = checked";
    style = ",type:'histogram'";
    title_YAxis = "Haeufigkeit";
  } else {
    style = ",mode:'" + plotstyle + "',type:'scatter'";
    YAxis = ",y: " + yColumn;
    // String erzeugen, um ein Dropdownmenue fuer die Y - Achsenwahl ausgeben zu
    // lassen (bei Histogramm nur X - Achse auswaehlbar):
    DropdownMenue_Y =
        "<form style=\"display: inline;\" action =\"/setYColumn\" method=\"post\" enctype=\"multipart/form-data\">\
           <label for=\"Spaltenwahl\">Y-Achse waehlen:</label>\
          <select name=\"spalte\" id=\"id_yspalte\" onchange=\"submit()\">\
            " +
        SPALTENOPTIONEN_Y + "</select>\
        </form>";
    title_YAxis = csv->columns[chosen_columns[1]].name;
  }

  std::string visualizationHttp =
      "<head><title>Messdaten visualisieren</title><script src='plotly-2.12.1.min.js'></script> \
                                    </head><body><div id='myDiv'></div></body> \
                                    <script>var trace1 = { \
                                        x: " +
      xColumn + YAxis + style + "}; \
                                        var data = [trace1]; \
                                        var layout = {\
                                        height: 800, \
                                        autosize: true, \
                                          title: ' " +
      chosen_file + "', \
                                         xaxis: {\
                                          title: '" +
      title_XAxis +

      "'},\
                                         yaxis: {\
                                          title: '" +
      title_YAxis +

      "'}\
                                        }; \
                                        Plotly.newPlot('myDiv', data, layout); \
                                    </script>\
      <form style=\"display: inline;\" action =\"/setXColumn\" method=\"post\" enctype=\"multipart/form-data\">\
          <label for=\"Spaltenwahl\">X-Achse waehlen:</label>\
          <select name=\"spalte\" id=\"id_xspalte\" onchange=\"submit()\">\
            " +
      SPALTENOPTIONEN_X + "\
          </select>\
          </form>\
          &nbsp;&nbsp;&nbsp;\
" + DropdownMenue_Y +
      "\
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
      <form style=\"display: inline;\" action =\"/plotstyle\" method=\"post\" enctype=\"multipart/form-data\">\
<input type=\"radio\" id=\"id_lineplot\" name=\"PlotStyle\" onclick=\"submit()\" value=\"lines+markers\" " +
      checked_lineplot + ">\
<label for=\"id_lineplot\">LinePlot</label>\
<input type=\"radio\" id=\"id_scatterplot \" name=\"PlotStyle\" onclick=\"submit()\" value=\"markers\" " +
      checked_scatterplot + ">\
<label for=\"id_scatterplot\">ScatterPlot</label>\
<input type=\"radio\" id=\"id_histogram\" name=\"PlotStyle\" onclick=\"submit()\" value=\"histogram\" " +
      checked_histogram + ">\
<label for=\"id_histogram\">Histogramm</label>\
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