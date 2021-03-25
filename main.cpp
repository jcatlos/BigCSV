#include <iostream>
#include <string>
#include <vector>

#include "csvFile.hpp"
#include "csvTable.hpp"


int main() {
    bigCSV::csvFile file("input.csv", ',', '\n', '"');
    std::vector<std::string> printed_columns;

    printed_columns.push_back("First name");
    file.printColumns(printed_columns);

    //run(std::cin);

    return 0;
}
