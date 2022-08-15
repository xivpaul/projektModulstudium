#include "Server.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "CSV.hpp"

const std::string WEB_ROOT = "../web_root";
const std::string DB_DIR = "../db/";

Server::Server() {
    
}

Server* Server::getInstance() {
    if (instance == nullptr) {
        instance = new Server();
    }
    return instance;
}

Server* Server::instance = nullptr;

static void http_callback(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        //MG_INFO(("New request to: [%.*s], body size: %lu", (int) hm->uri.len, hm->uri.ptr, (unsigned long) hm->body.len));
    
        if (mg_http_match_uri(hm, "/upload")) {
            struct mg_http_part part;
            size_t ofs = 0;
            while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
                //MG_INFO(("Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
                //    (int) part.name.len, part.name.ptr, (int) part.filename.len,
                //    part.filename.ptr, (unsigned long) part.body.len));
                //MG_INFO(("Data: %.*s", (int) part.body.len, part.body.ptr));

                std::string filename(part.filename.ptr, part.filename.len);
                std::string data(part.body.ptr, part.body.len);

                /* ignore filename and just replace data.csv as long as we have no file list */
                Server::getInstance()->handleCSVFileUpload(data);
            }

            std::string redirection = "<head><meta http-equiv=\"Refresh\" content=\"3; URL=/\"></head><body>Upload erfolgreich! Sie werden gleich wieder zur Hauptseite gebracht.</body>";
            mg_http_reply(c, 303, "", redirection.c_str());
        } else if (mg_http_match_uri(hm, "/analyse")) { 
            std::string result = Server::getInstance()->handleAnalysisRequest();
            mg_http_reply(c, 200, "", result.c_str());
        } else if (mg_http_match_uri(hm, "/visualize")) { 
            std::string result = Server::getInstance()->handleVisualizationRequest();
            mg_http_reply(c, 200, "", result.c_str());
        } else {
            struct mg_http_serve_opts opts = {.root_dir = WEB_ROOT.c_str()};
            mg_http_serve_dir(c, (struct mg_http_message *) ev_data, &opts);
        }
    }
}

void Server::handleCSVFileUpload(std::string data) {
    std::ofstream outfile(DB_DIR + "data.csv");
    outfile << data;
    outfile.close();
}

std::string Server::handleAnalysisRequest() {
    CSV csv;

    csv.read(DB_DIR + "data.csv");
    double result = csv.columns[0].mean(); 

    return "Ergebnis der Analyse: \nMittelwert der Spalte 0 = " + std::to_string(result);
}

std::string Server::handleVisualizationRequest() {
    /* todo: actually, instead of fixed values for x and y the data should be loaded from the CSV file */
    CSV csv;

    csv.read(DB_DIR + "data.csv");

    std::string xColumn = csv.columns[1].toString();
    std::string yColumn = csv.columns[2].toString();

    std::string visualizationHttp = "<head><script src='plotly-2.12.1.min.js'></script> \
                                    </head><body><div id='myDiv'></div></body> \
                                    <script>var trace1 = { \
                                        x: " + xColumn + /* [1, 2, 3, 4]*/ ", \
                                        y: " + yColumn + /* [10, 15, 13, 17]*/ ", \
                                        mode: 'markers', \
                                            type: 'scatter' \
                                        }; \
                                        var data = [trace1]; \
                                        Plotly.newPlot('myDiv', data); \
                                    </script>";

    return visualizationHttp;
}

void Server::start() {
    struct mg_mgr mgr;

    mg_mgr_init(&mgr);
    mg_log_set("3");
    mg_http_listen(&mgr, "http://localhost:8000", http_callback, NULL);

    for (;;) {
        mg_mgr_poll(&mgr, 50);
    }
  
    mg_mgr_free(&mgr);
}