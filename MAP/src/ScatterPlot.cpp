#include "ScatterPlot.hpp"
#include <string>

ScatterPlot::ScatterPlot() {}

std::string ScatterPlot::plot(std::string DB_DIR_Input,
                              std::string chosen_file) {
  compute(DB_DIR_Input, chosen_file);

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
                                          title: 'y-axis title'}\
                                        }; \
                                        Plotly.newPlot('myDiv', data, layout); \
                                    </script>\
                                    <button onclick=\"history.back()\">Go Back</button>";
  return visualizationHttp;
}