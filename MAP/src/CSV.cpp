#include "CSV.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

void CSV::read(std::string path) {
    nRows = 0;
    std::ifstream csv_file(path);

    std::string line;
    if (csv_file.is_open()) {
        while (csv_file) {
            csv_file >> line;

            std::stringstream ss(line);
            std::vector<CSVValue> parsed;

            for (std::string token; std::getline(ss, token, ',');) {
                if (token.size() >= 2 && token[0] == '"' && token[token.size()-1] == '"') {
                    token = token.substr(1, token.size() - 2);

                    parsed.push_back(CSVValue(token));
                } else {
                    parsed.push_back(CSVValue(std::stod(token)));
                }
            }

            if (columns.size() == 0) {
                bool allStrings = true;
                for (CSVValue csvValue: parsed) {
                    if (csvValue.type != STRING) {
                        allStrings = false;
                        break;
                    }
                }

                for (CSVValue csvValue: parsed) {
                    if (allStrings) {
                        columns.push_back(Column(csvValue.stringValue));
                    } else {
                        Column column;
                        column.values.push_back(csvValue);
                        columns.push_back(column);
                        nRows++;
                    }
                }

                continue;
            }

            int nrColumn = 0;
            for (CSVValue csvValue: parsed) {
                columns[nrColumn++].values.push_back(csvValue);
            }
            nRows++;
        }
    }
}

void CSV::print() {
    std::cout << "| ";
    for (Column column: columns) {
        std::cout << column.name << " | ";
    }
    std::cout << std::endl;

    for (int row; row < nRows; row++) {
        std::cout << "| ";
        for (Column column: columns) {
            if (column.values[row].type == NUMBER) {
                std::cout << column.values[row].numberValue;
            } else {
                std::cout << column.values[row].stringValue;
            }
            std::cout << " | ";
        }
        std::cout <<std::endl;
    }
}