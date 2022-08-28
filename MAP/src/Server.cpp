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
CSV csv;

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
      // Die gewaehlte Datei wird direkt eingelesen, damit verfuegbare
      // Spaltennamen angezeigt werden koennen:
      csv.read(DB_DIR + Server::getInstance()->chosen_file);

      // Weboberflaeche wird nach 2s neu geladen und aktualisiert:
      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"2; "
          "URL=/\"></head><body>Upload erfolgreich! Sie werden gleich wieder "
          "zur Hauptseite gebracht.</body>";
      mg_http_reply(c, 303, "", redirection.c_str());

    } else if (mg_http_match_uri(hm, "/metadata")) {
      std::string result = Server::getInstance()->handleMetadataRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/visualize")) {
      std::string result = Server::getInstance()->handleVisualizationRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/analyse")) {
      std::string result = Server::getInstance()->handleAnalysisRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/")) {
      std::string result = Server::getInstance()->handleStartPageRequest();
      mg_http_reply(c, 200, "", result.c_str());
    } else if (mg_http_match_uri(hm, "/setXColumn") or
               mg_http_match_uri(hm, "/setYColumn")) {
      struct mg_http_part part;
      size_t ofs = 0;
      std::string chosen_column;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        std::string filename_server(part.body.ptr);
        chosen_column = filename_server.substr(0, (int)part.body.len);
      }
      if (mg_http_match_uri(hm, "/setXColumn")) {
        scattplot.chosen_columns[0] = stoi(chosen_column);
      } else {
        scattplot.chosen_columns[1] = stoi(chosen_column);
      }
      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"0; "
          "URL=/visualize\"></head>";
      mg_http_reply(c, 200, "", redirection.c_str());

    } else if (mg_http_match_uri(hm, "/plotstyle")) {
      struct mg_http_part part;
      size_t ofs = 0;
      int i = 0;
      std::string plotstyle;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        std::string filename_server(part.body.ptr);
        plotstyle = filename_server.substr(0, (int)part.body.len);
        scattplot.plotstyle = plotstyle;
      }
      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"0; "
          "URL=/visualize\"></head>";
      mg_http_reply(c, 200, "", redirection.c_str());
    } else if (mg_http_match_uri(hm, "/load")) {
      struct mg_http_part part;
      size_t ofs = 0;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        std::string filename_server(part.body.ptr);
        Server::getInstance()->chosen_file =
            filename_server.substr(0, (int)part.body.len);
      }
      csv.read(DB_DIR + Server::getInstance()->chosen_file);

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

/**
 * @brief Die Methode ermittelt die im Datenbankverzeichnis vorhandenen Dateien
 * und speichert sie in einen Stringvektor. Es wird ein String zur Ausgabe an
 * mongoose zur Erzeugung der Weboberflaeche erstellt, in dem die gefundenen
 * Daten im Dropdownmenue ausgewaehlt werden koennen.
 *
 * @return std::string HTML String mit Dropdown - Menue zur Ausgabe an mongoose
 */
std::string Server::handleStartPageRequest() {
  // CSV csv;
  std::string httpStartPageString;
  std::vector<std::string> data_vector;
  auto loaded_file = Server::getInstance()->chosen_file;

  // Vorhandene Daten unter Verzeichnis "DB_DIR" suchen:
  const std::filesystem::path database_path{DB_DIR};
  for (auto const &dir_entry :
       std::filesystem::directory_iterator{database_path}) {
    std::string filename = dir_entry.path().filename().string();

    // die Dateinamen werden im Stringvektor gepspeichert, wenn Sie nicht der
    // gerade geladenen Datei entsprechen und auf "csv" enden:
    if (filename != loaded_file and
        filename.substr(filename.length() - 3, filename.length()) == "csv") {
      data_vector.push_back(filename);
    }
  }

  // String definieren fuer Eintraege im Dropdownmenue:
  std::string OPTIONSLISTE;

  // wenn eine Datei geladen ist (Endung auf "csv"), dann soll der erste Eintrag
  // im Auswahlmenue dieser Datei entsprechen:
  if (loaded_file.substr(loaded_file.length() - 3, loaded_file.length()) ==
      "csv") {
    OPTIONSLISTE =
        "<option value=" + loaded_file + ">" + loaded_file + "</option>";
  }
  // Erzeugung des Dropdownmenues zur Auswahl der vorhandenden Messdateien fuer
  // HTML:
  for (std::string file : data_vector) {
    OPTIONSLISTE += ("<option value=" + file + ">" + file + "</option>");
  }
  // Erzeugen eines HTML Strings mit der gerade erzeugten OPTIONSLISTE:
  httpStartPageString = Server::modifyHTMLText("OPTIONSLISTE", OPTIONSLISTE);

  // Es wird ein Text der gerade geladenen Datei angezeigt. Dazu wird der String
  // nochmals modifiziert (hier zusaetzlich uebergabe des zu modifizierenden
  // Stringobjekts "httpStartPageString"):
  httpStartPageString = Server::modifyHTMLText(
      "USERTEXT", "Gewaehlte Messdatei: " + loaded_file, httpStartPageString);

  // Metadaten
  httpStartPageString = Server::modifyHTMLText(
      "METADATEN", handleMetadataRequest(), httpStartPageString);

  // zwischengespeicherten HTML - String aktualisieren:
  Server::getInstance()->currentHTMLString = httpStartPageString;
  return httpStartPageString;
}

/**
 * @brief Die Methode findet in der Urprungs - HTML - Datei "index.html" oder
 * optional in einem mitgegebenen Stringobjekt ein Schlagwort und ersetzt dieses
 * durch den mitgegebenen Ersatztext.
 *
 * @param Schlagwort zu ersetzendes Wort
 * @param Ersatztext ersetzender Text
 * @param Stringobjekt optionaler Parameter
 * @return std::string
 */
std::string Server::modifyHTMLText(std::string Schlagwort,
                                   std::string Ersatztext,
                                   std::string Stringobjekt) {
  // Quelle fuer String replace:
  // https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c

  std::string HTMLString;
  // wenn ein String mitgegeben wird, wird dieser veraendert, sonst wird das
  // mitgegebene Stringobjekt geaendert:
  if (Stringobjekt == "empty") {
    std::ifstream inFile(WEB_ROOT + "/index.html");
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    HTMLString = buffer.str();
  } else {
    HTMLString = Stringobjekt;
  }
  HTMLString =
      std::regex_replace(HTMLString, std::regex(Schlagwort), Ersatztext);
  return HTMLString;
}

/**
 * @brief Die Methode erstellt eine csv - Datei unter dem in "DB_DIR"
 * hinterlegten Verzeichnis mit dem Namen entsprechend der ausgewaehlten Datei
 * und deren Inhalt
 *
 * @param data
 */
void Server::handleCSVFileUpload(std::string data) {

  std::ofstream outfile(DB_DIR + Server::getInstance()->chosen_file);
  outfile << data;
  outfile.close();
  Server::createMetadata();
}

void Server::createMetadata() {
  // CSV csv;
  csv.setMetadata(DB_DIR + Server::getInstance()->chosen_file);

  std::ofstream outfile(DB_DIR + Server::getInstance()->chosen_file + "meta");
  outfile << "Pfad: " + csv.metadata[0] + "<br>";
  outfile << "Format: " + csv.metadata[1] + "<br>";
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
  if (csv.columns.size() < 1) {
    std::string redirection = "<head><meta http-equiv=\"Refresh\" content=\"4; "
                              "URL=/\"></head><body>Bitte Messreihe "
                              "waehlen! Sie werden gleich wieder "
                              "zur Hauptseite gebracht.</body>";
    return redirection;
  }
  // double result = csv.columns[0].mean();
  csv.buildAnalysisMatrix();

  // return "Ergebnis der Analyse: \nMittelwert der Spalte 0 = " +
  //  std::to_string(result);
  return scattplot.showAnalysis(csv, Server::getInstance()->chosen_file);
}

std::string Server::handleVisualizationRequest() {
  if (csv.columns.size() < 1) {
    std::string redirection = "<head><meta http-equiv=\"Refresh\" content=\"4; "
                              "URL=/\"></head><body>Bitte Messreihe "
                              "waehlen! Sie werden gleich wieder "
                              "zur Hauptseite gebracht.</body>";
    return redirection;
  }
  return scattplot.plot(DB_DIR, Server::getInstance()->chosen_file, csv);
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