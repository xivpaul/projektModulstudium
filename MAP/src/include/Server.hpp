#pragma once

#include <string>
#include "mongoose.h"

class Server {
private:
    static Server* instance;

    Server();

public:
    static Server* getInstance();

    Server(Server &other) = delete; // C++11 prevent cloning
    void operator=(const Server &) = delete; // C++11 prevent assignments

    void handleCSVFileUpload(std::string data);
    std::string handleAnalysisRequest();
    std::string handleVisualizationRequest();

    void start();
};