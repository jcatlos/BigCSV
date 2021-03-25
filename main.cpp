#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

#include "csvFile.hpp"
#include "csvTable.hpp"
#include "helper.hpp"


int main() {
    bigCSV::csvFile file("input.csv", ',', '\n', '"');
    std::vector<std::string> printed_columns;

    printed_columns.push_back("First name");
    file.printColumns(printed_columns);

    //run(std::cin);

    return 0;
}
