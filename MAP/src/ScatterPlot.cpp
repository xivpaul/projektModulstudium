#include "ScatterPlot.hpp"
// #include <iostream>
#include <string>

ScatterPlot::ScatterPlot() {}

std::string ScatterPlot::plot(std::string DB_DIR_Input, std::string chosen_file,
                              int y_column, CSV csv) {
  compute(DB_DIR_Input, chosen_file, y_column);
  // Scatterplot: mode: 'lines+markers'
  // Lineplot: mode: 'lines+markers'

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
                                          title: 'x-axis title'},\
                                         yaxis: {\
                                          title: '" +
      csv.columns[y_column].name +

      "'}\
                                        }; \
                                        Plotly.newPlot('myDiv', data, layout); \
                                    </script>\
                                    <button onclick=\"history.back()\">Go Back</button>";
  return visualizationHttp;
}