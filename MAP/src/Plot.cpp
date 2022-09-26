#include "Plot.hpp"
// Konstruktor:
Plot::Plot() {}

/**
 * @brief Die Methode gibt einen HTML String aus, um auf der Weboberfläche
 * die Messdaten zu visualisieren, sowie eine Achsauswahl der X- und Y-Achse
 * vornehmen zu können. Zudem koennen Plotoptionen eingestellt werden.
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
                       std::string chosen_file, std::string infoAlert) {
  if (plotstyle == "filteractive") {
    chosen_columns[0] = 0;
  }
  compute(csv, DB_DIR_Input, chosen_file, chosen_columns);
  auto SPALTENOPTIONEN_X = setXColumnOptions(csv, chosen_file, chosen_columns);
  auto SPALTENOPTIONEN_Y = setYColumnOptions(csv, chosen_file, chosen_columns);

  std::string checked_lineplot = "";
  std::string checked_scatterplot = "";
  std::string checked_histogram = "";
  std::string YAxis = "";
  std::string style;
  std::string title_XAxis = csv->columns[chosen_columns[0]].name;
  std::string title_YAxis = "";
  std::string checked_filteractive = "";
  std::string YAxis_filtered = "";
  std::string Eingabefenster_Filter = "";
  std::string enable_x = "";
  std::string enable_y = "";
  std::string result_data, result_time;
  std::string legend_options = "";
  this->smoothed_data = "";
  if (plotstyle == "markers") {
    checked_scatterplot = "checked = checked";
  }
  if (plotstyle == "lines+markers") {
    checked_lineplot = "checked = checked";
  }
  if (plotstyle == "histogram") {
    checked_histogram = "checked = checked";
    style = ",type:'histogram'";
    title_YAxis = "Häufigkeit";
    enable_y = "disabled=\"disabled\"";
    this->smoothed_data = "";
  } else {
    style = ",mode:'" + plotstyle + "',type:'scatter',name:'Rohdaten'";
    if (plotstyle == "filteractive") {
      enable_x = "disabled=\"disabled\"";
      checked_filteractive = "checked=checked";
      legend_options = "showlegend: true, legend: {\"orientation\": "
                       "\"h\",\"y\":1.1,\"x\":0.75,\"font\":{size:15}},";
      title_XAxis = csv->columns[chosen_columns[0]].name;
      // Datenglaettung
      this->calcFilteredData(csv, filtersize, &result_data, &result_time);
      this->smoothed_data = " x: [" + result_time + "],y: [" + result_data +
                            "],mode:'lines',name:'Datenglättung mit gleitendem "
                            "Mittelwert',opacity: '15' ,type:'scatter'";
      Eingabefenster_Filter =
          "<form style=\"display:inline;\" action=\"/setFilterValue\" method=\"post\" enctype=\"multipart/form-data\">\
                <input type=\"text\" id=\"id_filtervalue\" \
         name=\"transformvalue\" value=" +
          std::to_string(filtersize) + ">\
          <button type=\"submit()\">Filtern</button>\
      </form>\
      <br>";
    }
    YAxis = ",y: " + yColumn;
    title_YAxis = csv->columns[chosen_columns[1]].name;
  }

  std::string visualizationHttp =
      "<!DOCTYPE html>\
<html lang=\"en\">\
 <meta charset=\"utf-8\" />\
  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\
    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\">\
  <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js\"></script>\
  <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js\"></script>\
  <link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\">\
<link\
    href=\"https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&display=swap\"\
    rel=\"stylesheet\">\
  <head>\
    <title>Messdaten analysieren</title>\
    <script src='plotly-2.12.1.min.js'></script> \
    <style>\
      #container {\
      margin-right: auto;\
      margin-left: auto;\
      width: auto;\
    }\
       #frame_axis {\
       margin-top:0px;\
      padding-top: 2em;\
      padding-left: 2em;\
      padding-right: 2em;\
      float: left;\
      width: 50%;\
      flex: 1;\
    }\
      #frame_plotstyle {\
      margin-top:0px;\
      padding-top: 2em;\
      padding-left: 2em;\
      padding-right: 2em;\
      float: right;\
      width: 50%;\
      flex: 1;\
    }\
     #frame {\
      margin-bottom: 0px;\
      margin-left: 10px;\
      margin-right: 10px;\
      border-radius: .5em;\
      padding-top: 2em;\
      padding-left: 2em;\
      padding-right: 2em;\
      padding-bottom: .8em;\
      border: 1px solid black;\
      display:flex;\
    }\
    body {\
color: black;\
}\
    </style>\
  </head>\
  <body>\
  <div id=\"container\">" +
      infoAlert +
      "<h1 style =\"margin-bottom:0px;text-align:center\"> Datenanalyse der "
      "Messdatei \"" +
      chosen_file + "\"</h1>\
  <div id='myDiv' style=\"margin-top:0px;\">\
  <script>var trace1 = { \
    x: " +
      xColumn + YAxis + style + "}; \
    var smoothed_data = {" +
      this->smoothed_data + "}; \
    var data = [trace1,smoothed_data]; \
    var layout = {" +
      legend_options + "height: 600, \
      autosize: true, \
      xaxis: {title: '" +
      title_XAxis + "'},\
      yaxis: {title: '" +
      title_YAxis + "'}\
    }; \
    Plotly.newPlot('myDiv', data, layout); \
  </script>\
  </div>\
  <div id=\"frame\">\
      <h3 style=\"margin-top:0px;\">Analyseoptionen</h3>\
    <div id='frame_axis'>\
      <h4>Achsbelegung festlegen:</h4>\
      <form action =\"/setXColumn\" method=\"post\" enctype=\"multipart/form-data\">\
        <label for=\"Spaltenwahl\">X-Achse wählen:</label>\
        <select name=\"spalte\" id=\"id_xspalte\" onchange=\"submit()\" " +
      enable_x + ">" + SPALTENOPTIONEN_X + "</select>\
      </form><br>\
      <form action =\"/setYColumn\" method=\"post\" enctype=\"multipart/form-data\">\
        <label for=\"Spaltenwahl\">Y-Achse wählen:</label>\
        <select name=\"spalte\" id=\"id_yspalte\" onchange=\"submit()\" " +
      enable_y + ">" + SPALTENOPTIONEN_Y + "</select>\
      </form>\
    </div>\
    <div id='frame_plotstyle'>\
      <h4>Plotoptionen festlegen:</h4>\
      <form action =\"/plotstyle\" method=\"post\" enctype=\"multipart/form-data\">\
        <input type=\"radio\" id=\"id_lineplot\" name=\"PlotStyle\" onclick=\"submit()\" value=\"lines+markers\" " +
      checked_lineplot + ">\
        <label for=\"id_lineplot\">LinePlot</label><br>\
        <input type=\"radio\" id=\"id_scatterplot \" name=\"PlotStyle\" onclick=\"submit()\" value=\"markers\" " +
      checked_scatterplot + ">\
        <label for=\"id_scatterplot\">ScatterPlot</label><br>\
        <input type=\"radio\" id=\"id_histogram\" name=\"PlotStyle\" onclick=\"submit()\" value=\"histogram\" " +
      checked_histogram + ">\
        <label for=\"id_histogram\">Histogramm</label><br>\
        </form>\
        <div>\
        <form style=\"display:inline;\" action =\"/plotstyle\" method=\"post\" enctype=\"multipart/form-data\">\
        <input type=\"radio\" id=\"id_filteractive \" name=\"PlotStyle\" onclick=\"submit()\" value=\"filteractive\" " +
      checked_filteractive + ">\
        <label for=\"id_filteractive\">Filter anwenden</label>\
    </form>" +
      Eingabefenster_Filter + "</div>\
    </div>\
  </div>\
  <br>\
  <form style=\"display: inline; margin-top:5px; margin-left: 10px; height=30px;\"action = \"http://localhost:8000\">\
<input id='backHome' style=\"color: white;\" type = \"submit\" value = \"Zurück zur Startseite\"/>\
</form>\
</div>\
</body>";
  return visualizationHttp;
}
// CSV Filterung mit gleitendem Mittelwert
void Plot::calcFilteredData(CSV *csv, int filtersize, std::string *result_data,
                            std::string *result_time) {

  double vector_neu[csv->nRows][2] = {0};
  for (int n = 0; n < csv->nRows; n++) {
    float temp_data = 0; // Temporaerer Datencontainer
    if (n < filtersize / 2 || n >= csv->nRows - (filtersize / 2)) {
      vector_neu[n][0] = -1; // Platzhalter für leere Zeilen
    } else {
      int ii = 0;
      ii = n - (filtersize / 2);
      for (ii; ii <= n + (filtersize / 2); ii++) {
        temp_data =
            temp_data +
            csv->columns[this->chosen_columns[1]].values[ii].numberValue;
      }
      vector_neu[n][1] = temp_data / filtersize;
      vector_neu[n][0] =
          csv->columns[this->chosen_columns[0]].values[n].numberValue;
      temp_data = 0;
    }
  }

  bool first = true;
  for (int n = 0; n < csv->nRows; n++) {
    if (!first) {
      *result_data = *result_data + ",";
      *result_time = *result_time + ",";
    }
    if (vector_neu[n][0] == -1) {
      std::string s_data = ",";
      std::string s_time = ",";
      *result_data = *result_data + s_data;
      *result_time = *result_time + s_time;
    } else {
      double data = vector_neu[n][1];
      std::string s_data = std::to_string(data);
      *result_data = *result_data + s_data;
      double time = vector_neu[n][0];
      std::string s_time = std::to_string(time);
      *result_time = *result_time + s_time;
      first = false;
    }
  }
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