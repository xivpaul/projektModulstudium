#include "Server.hpp"
#include "CSV.hpp"
#include "ScatterPlot.hpp"
#include "Visualization.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

const std::string WEB_ROOT = "../web_root";
const std::string DB_DIR = "../db/";

Server::Server() {}

ScatterPlot scattplot;

Server *Server::getInstance() {
  if (instance == nullptr) {
    instance = new Server();
  }
  return instance;
}

Server *Server::instance = nullptr;

static void http_callback(struct mg_connection *c, int ev, void *ev_data,
                          void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    // MG_INFO(("New request to: [%.*s], body size: %lu", (int) hm->uri.len,
    // hm->uri.ptr, (unsigned long) hm->body.len));

    if (mg_http_match_uri(hm, "/upload")) {
      struct mg_http_part part;
      size_t ofs = 0;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        // MG_INFO(("Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
        //    (int) part.name.len, part.name.ptr, (int) part.filename.len,
        //    part.filename.ptr, (unsigned long) part.body.len));
        // MG_INFO(("Data: %.*s", (int) part.body.len, part.body.ptr));

        std::string filename(part.filename.ptr, part.filename.len);
        std::string data(part.body.ptr, part.body.len);

        /* ignore filename and just replace data.csv as long as we have no file
         * list */
        Server::getInstance()->chosen_file = filename;
        Server::getInstance()->handleCSVFileUpload(data);
      }

      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"2; "
          "URL=/\"></head><body>Upload erfolgreich! Sie werden gleich wieder "
          "zur Hauptseite gebracht.</body>";
      mg_http_reply(c, 303, "", redirection.c_str());
    } else if (mg_http_match_uri(hm, "/metadata")) {
      std::string result = Server::getInstance()->handleMetadataRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/analyse")) {
      std::string result = Server::getInstance()->handleAnalysisRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/visualize")) {
      std::string result = Server::getInstance()->handleVisualizationRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/")) {
      std::string result = Server::getInstance()->handleStartPageRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/load")) {
      struct mg_http_part part;
      size_t ofs = 0;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        std::string filename_server(part.body.ptr);
        Server::getInstance()->chosen_file =
            filename_server.substr(0, (int)part.body.len);
      }
      // Die HTML wird neu geladen, damit die Aenderungen sichtbar werden:
      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"0; "
          "URL=/\"></head>";

      mg_http_reply(c, 200, "", redirection.c_str());
    } else {
      struct mg_http_serve_opts opts = {.root_dir = WEB_ROOT.c_str()};
      mg_http_serve_dir(c, (struct mg_http_message *)ev_data, &opts);
    }
  }
}

std::string Server::handleStartPageRequest() {
  std::string httpStartPageString;
  std::ifstream inFile(WEB_ROOT + "/index.html");
  std::stringstream buffer;
  buffer << inFile.rdbuf();
  std::vector<std::string> data_vector;
  const std::filesystem::path database_path{DB_DIR};
  auto loaded_file = Server::getInstance()->chosen_file;

  // directory_iterator can be iterated using a range-for loop
  for (auto const &dir_entry :
       std::filesystem::directory_iterator{database_path}) {
    std::string filename = dir_entry.path().filename().string();
    if (filename != loaded_file and
        filename.substr(filename.length() - 3, filename.length()) == "csv") {
      data_vector.push_back(filename);
    }
  }
  std::string OPTIONSLISTE;

  if (loaded_file.substr(loaded_file.length() - 3, loaded_file.length()) ==
      "csv") {
    OPTIONSLISTE =
        "<option value=" + loaded_file + ">" + loaded_file + "</option>";
  }

  // std::string OPTIONSLISTE = "<option value=\" - \"> - </option>";
  for (std::string file : data_vector) {
    OPTIONSLISTE += ("<option value=" + file + ">" + file + "</option>");
  }
  // Quelle:
  // https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
  httpStartPageString = buffer.str();
  httpStartPageString = std::regex_replace(
      httpStartPageString, std::regex("OPTIONSLISTE"), OPTIONSLISTE);

  httpStartPageString = std::regex_replace(
      httpStartPageString, std::regex("USERTEXT"),
      "Gewaehlte Messreihe: " + Server::getInstance()->chosen_file);

  return httpStartPageString;
}

void Server::handleCSVFileUpload(std::string data) {
  std::ofstream outfile(DB_DIR + Server::getInstance()->chosen_file);
  outfile << data;
  outfile.close();

  Server::createMetadata();
}

void Server::createMetadata() {
  CSV csv;

  csv.setMetadata(DB_DIR + Server::getInstance()->chosen_file);

  std::ofstream outfile(DB_DIR + Server::getInstance()->chosen_file + "meta");
  outfile << "Pfad: " + csv.metadata[0] + "\n";
  outfile << "Format: " + csv.metadata[1] + "\n";
  outfile << "Datum: " + csv.metadata[2];
  outfile.close();
}

std::string Server::handleMetadataRequest() {

  std::ifstream text(DB_DIR + Server::getInstance()->chosen_file + "meta");
  std::stringstream buffer;
  buffer << text.rdbuf();

  return buffer.str();
}

std::string Server::handleAnalysisRequest() {
  CSV csv;
  csv.read(DB_DIR + Server::getInstance()->chosen_file);
  double result = csv.columns[0].mean();

  return "Ergebnis der Analyse: \nMittelwert der Spalte 0 = " +
         std::to_string(result);
}

std::string Server::handleVisualizationRequest() {
  return scattplot.plot(DB_DIR, Server::getInstance()->chosen_file);
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