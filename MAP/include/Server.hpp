#pragma once

#include "mongoose.h"
#include <string>

class Server {
private:
  static Server *instance;

  Server();

public:
  int y_column;
  std::string currentHTMLString;
  static Server *getInstance();
  std::string chosen_file = "Keine Datei ausgewaehlt";
  Server(Server &other) = delete;          // C++11 prevent cloning
  void operator=(const Server &) = delete; // C++11 prevent assignments
  std::string modifyHTMLText(std::string Schlagwort, std::string Ersatztext,
                             std::string Stringobject = "empty");
  void handleCSVFileUpload(std::string data);
  void createMetadata();
  std::string handleMetadataRequest();
  std::string handleAnalysisRequest();
  std::string handleVisualizationRequest();
  std::string handleStartPageRequest();
  std::string handleColumnSetRequest(std::string Stringobject);
  void start();
};