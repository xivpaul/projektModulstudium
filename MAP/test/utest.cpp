#define BOOST_TEST_MAIN
#include "CSV.hpp"

#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <vector>

BOOST_AUTO_TEST_CASE(check_metadaten_1) {
  CSV csv;
  csv.setMetadata("../db/weekly.csv");
  BOOST_CHECK(csv.metadata[0] == "../db/weekly.csv");
  BOOST_CHECK(csv.metadata[1] == "csv");
  std::cout << csv.metadata[2] << std::endl;
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
  std::string timestamp(buffer);
  BOOST_CHECK(csv.metadata[2] == timestamp);
}

BOOST_AUTO_TEST_CASE(check_metadaten_2) {
  CSV csv;
  csv.setMetadata("../db/ .xlsx");
  BOOST_CHECK(csv.metadata[0] == ("../db/ .xlsx"));
  BOOST_CHECK(csv.metadata[1] == ("xlsx"));
}

BOOST_AUTO_TEST_CASE(check_metadaten_3) {
  CSV csv;
  csv.setMetadata("../db/weekly");
  BOOST_CHECK(csv.metadata[0] == ("../db/weekly"));
  BOOST_CHECK(csv.metadata[1] == (""));
  std::cout << "Test schlaegt fehl, weil der Code die fehlende Format-Endung "
               "nicht abfangen kann. Als Endung wird das hier erzeugt: "
            << csv.metadata[1] << std::endl;
}

BOOST_AUTO_TEST_CASE(check_metadaten_4) {
  CSV csv;
  csv.setMetadata("../db/ ");
  BOOST_CHECK(csv.metadata[0] == ("../db/ "));
  BOOST_CHECK(csv.metadata[1] == (""));
  std::cout << "Test schlaegt fehl, weil der Code die fehlende Format-Endung "
               "nicht abfangen kann. Als Endung wird das hier erzeugt: "
            << csv.metadata[1] << std::endl;
}

BOOST_AUTO_TEST_CASE(check_csv_read_1) {
  CSV csv;
  csv.read("../db/test.csv");
  BOOST_CHECK(csv.columns.size() == 9 + 1);
  BOOST_CHECK(csv.columns[0].name == "Index Messpunkte");
  BOOST_CHECK(csv.columns[9].name == "Direction");
  BOOST_CHECK(csv.columns[0].values.size() == 573 + 1);
  BOOST_CHECK(csv.columns[1].values[1].numberValue ==
              2000); // Erster Wert in erster Spalte
  BOOST_CHECK(csv.columns[9].values[573].stringValue ==
              "Up"); // Letzter Wert in letzter Spalte
}
