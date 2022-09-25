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
  BOOST_CHECK(csv.metadata[2] == "17-09-2022 09:58:33");
}

BOOST_AUTO_TEST_CASE(check_metadaten_2) {
  CSV csv;
  csv.setMetadata("../db/something_else.xlsx");
  BOOST_CHECK(csv.metadata[0] == ("../db/something_else.xlsx"));
  BOOST_CHECK(csv.metadata[1] == ("xlsx"));
  // BOOST_CHECK(metadata[2] == ("csv"));
}
