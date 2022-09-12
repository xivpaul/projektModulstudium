#include "Server.hpp"

const std::string WEB_ROOT = "../web_root";
const std::string DB_DIR = "../db/";

Server::Server() {}

Plot plotObj;
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

        std::string filename(part.filename.ptr, part.filename.len);
        std::string data(part.body.ptr, part.body.len);

        // Vorhandene Daten unter Verzeichnis "DB_DIR" suchen:
        bool already_there = false;
        auto file_to_load = filename;
        const std::filesystem::path database_path{DB_DIR};
        for (auto const &dir_entry :
             std::filesystem::directory_iterator{database_path}) {
          std::string filename = dir_entry.path().filename().string();

          // Dateinamen werden verglichen. Wenn Dateiname schon vorhanden wird
          // ein Flag gesetzt.
          if (filename == file_to_load) {
            already_there = true;
          }
        }
        // Bei existierender Datei wird zum erneuten Hochladen aufgefordert und
        // an die Startseite zurückgegeben.
        if (already_there) {
          std::string redirection =
              "<head><meta http-equiv=\"Refresh\" content=\"8; "
              "URL=/\"></head><body><h2>Upload fehlgeschlagen!</h2>"
              "Datei existiert bereits!<br>"
              "Bitte Datei umbenennen und erneut hochladen.<br>"
              "Sie werden gleich wieder zur Hauptseite gebracht.</body>";
          mg_http_reply(c, 303, "", redirection.c_str());
        } else {
          Server::getInstance()->chosen_file = filename;
          Server::getInstance()->handleCSVFileUpload(data);

          // Die gewaehlte Datei wird direkt eingelesen, damit verfuegbare
          // Spaltennamen angezeigt werden koennen:
          csv.read(DB_DIR + Server::getInstance()->chosen_file);

          // Weboberflaeche wird nach 2s neu geladen und aktualisiert:
          std::string redirection =
              "<head><meta http-equiv=\"Refresh\" content=\"2; "
              "URL=/\"></head><body>Upload erfolgreich! Sie werden gleich "
              "wieder "
              "zur Hauptseite gebracht.</body>";
          mg_http_reply(c, 303, "", redirection.c_str());
        }
      }

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
        plotObj.chosen_columns[0] = stoi(chosen_column);
      } else {
        plotObj.chosen_columns[1] = stoi(chosen_column);
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
        plotObj.plotstyle = plotstyle;
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
      plotObj.clearHistory();
      // Die HTML wird neu geladen, damit die Aenderungen sichtbar werden:
      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"0; "
          "URL=/\"></head>";
      mg_http_reply(c, 200, "", redirection.c_str());
      Server::getInstance()->handleDownloadRequest();

    } else if (mg_http_match_uri(hm, "/setOperation")) {
      struct mg_http_part part;
      size_t ofs = 0;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        std::string received_data(part.body.ptr);
        csv.TransformOperation = received_data.substr(0, (int)part.body.len);
      }

      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"0; "
          "URL=/\"></head>";
      mg_http_reply(c, 200, "", redirection.c_str());
    } else if (mg_http_match_uri(hm, "/transformColumn")) {
      struct mg_http_part part;
      size_t ofs = 0;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        std::string received_data(part.body.ptr);

        csv.ColumnToTransform =
            stoi(received_data.substr(0, (int)part.body.len));
      }

      std::string redirection =
          "<head><meta http-equiv=\"Refresh\" content=\"0; "
          "URL=/\"></head>";
      mg_http_reply(c, 200, "", redirection.c_str());
    } else if (mg_http_match_uri(hm, "/setTransformationValue")) {
      struct mg_http_part part;
      size_t ofs = 0;
      std::string redirection;
      // Benutzernachricht, wenn noch keine Messdatei ausgewaehlt wurde
      if (csv.columns.size() < 1) {
        redirection = "<head><meta http-equiv=\"Refresh\" content=\"3; "
                      "URL=/\"></head><body>Bitte wahelen Sie zunaechst eine "
                      "Messdatei aus!\n Sie werden gleich wieder "
                      "zur Hauptseite gebracht.</body>";
        mg_http_reply(c, 200, "", redirection.c_str());
      } else {
        while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
          std::string received_data(part.body.ptr);
          try {
            double TransformValue =
                stod(received_data.substr(0, (int)part.body.len));

            Server::getInstance()->handleTransformationRequest(TransformValue);
            redirection = "<head><meta http-equiv=\"Refresh\" content=\"0; "
                          "URL=/\"></head>";
            mg_http_reply(c, 200, "", redirection.c_str());
          } catch (...) {
            std::string redirection =
                "<head><meta http-equiv=\"Refresh\" content=\"3; "
                "URL=/\"></head><body>Transformationswert darf nicht leer sein "
                "und "
                "keine Buchstaben enthalten!\n Sie werden gleich wieder "
                "zur Hauptseite gebracht.</body>";
            mg_http_reply(c, 200, "", redirection.c_str());
          }
        }
      }

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
  auto loaded_file = Server::getInstance()->chosen_file;
  // Erzeugen eines HTML Strings mit der gerade erzeugten OPTIONSLISTE:
  std::string httpStartPageString = Server::modifyHTMLText(
      "OPTIONSLISTE", csv.createDropDownString_Files(loaded_file, DB_DIR));

  // Erzeugen eines HTML Strings mit der gerade erzeugten CURRENT_FILENAME:
  httpStartPageString = Server::modifyHTMLText("CURRENT_FILENAME", loaded_file,
                                               httpStartPageString);

  // Es wird ein Text der gerade geladenen Datei angezeigt. Dazu wird der String
  // nochmals modifiziert (hier zusaetzlich uebergabe des zu modifizierenden
  // Stringobjekts "httpStartPageString"):
  httpStartPageString = Server::modifyHTMLText(
      "USERTEXT", "Gewaehlte Messdatei: " + loaded_file, httpStartPageString);

  // Metadaten
  httpStartPageString = Server::modifyHTMLText(
      "METADATEN", handleMetadataRequest(), httpStartPageString);
  // Erzeugung des Dropdownmenues zur Auswahl der zu transformierenden Spalte
  // fuer HTML:

  httpStartPageString = Server::modifyHTMLText(
      "TRANSFORMATIONSSPALTE", csv.createDropDownString_Column(),
      httpStartPageString);

  httpStartPageString = Server::modifyHTMLText(
      "OPERATION", csv.createDropDownString_Operation(), httpStartPageString);

  httpStartPageString = Server::modifyHTMLText(
      "EINGABEWERT", csv.createInputValueString(), httpStartPageString);
  httpStartPageString = Server::modifyHTMLText(
      "TRANSFORMATIONSHISTORIE",
      plotObj.createTransformationHistoryTableString(), httpStartPageString);

  httpStartPageString =
      Server::modifyHTMLText("path_to_file", loaded_file, httpStartPageString);
  if (csv.columns.size() > 0) {
    std::string DownloadString = "<a href=\"download_folder/download.csv\" "
                                 "download=" +
                                 Server::getInstance()->chosen_file +
                                 ">Gewählte Messdatei herunterladen</a>";
    httpStartPageString = Server::modifyHTMLText("DOWNLOADLINK", DownloadString,
                                                 httpStartPageString);
  }

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
  csv.createMetadata(DB_DIR + Server::getInstance()->chosen_file);
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
  csv.buildAnalysisMatrix();
  return plotObj.showAnalysis(&csv, Server::getInstance()->chosen_file);
}

void Server::handleTransformationRequest(double TransformValue) {
  std::string pathToTranformedFile =
      DB_DIR + "Trans_" + Server::getInstance()->chosen_file;
  csv.TransformValue = TransformValue;
  csv.transformColumnValues(pathToTranformedFile);

  // Aktuelle Transformationseingeben in Vektoren schreiben fuer Anzeige in
  // Tabelle
  plotObj.trans_ColumnHistory.push_back(
      csv.columns[csv.ColumnToTransform].name);
  plotObj.trans_ValueHistory.push_back(std::to_string(csv.TransformValue));
  plotObj.trans_OperationHistory.push_back(csv.TransformOperation);
  plotObj.Transformationszaehler += 1;
}
void Server::handleDownloadRequest() {
  csv.downloadFileFromServer(WEB_ROOT, DB_DIR,
                             Server::getInstance()->chosen_file);
}

std::string Server::handleVisualizationRequest() {
  if (csv.columns.size() < 1) {
    std::string redirection = "<head><meta http-equiv=\"Refresh\" content=\"4; "
                              "URL=/\"></head><body>Bitte Messreihe "
                              "waehlen! Sie werden gleich wieder "
                              "zur Hauptseite gebracht.</body>";
    return redirection;
  }
  return plotObj.plot(&csv, DB_DIR, Server::getInstance()->chosen_file);
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
