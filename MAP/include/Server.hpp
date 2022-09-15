#pragma once

#include "CSV.hpp"
#include "Plot.hpp"
#include "Visualization.hpp"
#include "mongoose.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

class Server {
private:
  static Server *instance;
  Server();

public:
  std::string currentHTMLString;
  std::string createInfoMessage(std::string infotext, std::string InfoStatus);
  static Server *getInstance();
  std::string chosen_file = "Keine Datei ausgewaehlt";
  Server(Server &other) = delete;          // C++11 prevent cloning
  void operator=(const Server &) = delete; // C++11 prevent assignments
  std::string modifyHTMLText(std::string Schlagwort, std::string Ersatztext,
                             std::string Stringobject = "empty");
  std::string handleCSVFileUpload(std::string data, bool already_there,
                                  std::string filename);
  std::string handleMetadataRequest();
  std::string handleAnalysisRequest();
  std::string handleVisualizationRequest();
  std::string handleStartPageRequest();
  void handleDownloadRequest();
  void handleTransformationRequest(double TransformValue);
  void start();
};