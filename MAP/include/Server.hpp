#pragma once

#include "mongoose.h"
#include <string>

class Server {
private:
  static Server *instance;

  Server();

public:
  static Server *getInstance();
  std::string chosen_file;
  Server(Server &other) = delete;          // C++11 prevent cloning
  void operator=(const Server &) = delete; // C++11 prevent assignments

  void handleCSVFileUpload(std::string data);
  void createMetadata(std::string m_filename);
  std::string handleMetadataRequest(std::string m_filename);
  std::string handleAnalysisRequest();
  std::string handleVisualizationRequest();
  std::string handleStartPageRequest();
  void start();
};