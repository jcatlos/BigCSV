#include <iostream>
#include <string>
#include <fstream>

#include "csvFile.hpp"

int main() {
    bigCSV::csvFile file("input.csv", ',', '\n', '"');
    //file.printFile();
    std::cout<<"Im alive"<<std::endl;
    return 0;
}
