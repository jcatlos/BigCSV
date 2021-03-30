#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "csvFile.hpp"
#include "csvTable.hpp"
#include "tmpFileFactory.hpp"
#include "tmpFile.hpp"


int main() {

    /*
    //std::cout<<std::filesystem::temp_directory_path()<<std::endl;
    bigCSV::csvTable table;
    bigCSV::csvFile file("input2.csv", ',', '\n', '"', false);
    std::vector<std::string> printed_columns;
    printed_columns.push_back("NazevZarizeni");
    printed_columns.push_back("Obec");
    //file.printColumns(printed_columns);
    //file.trivialSort(printed_columns);

    auto files = file.distribute();
    std::cout<<"first: "<<files.front().file_name<<" size: "<<files.size()<<std::endl;
    files[1].printColumns(printed_columns);

     */

    std::vector<bigCSV::tmpFile> tmpFiles;
    for(int i=0; i<100; i++){
        tmpFiles.push_back(bigCSV::tmpFileFactory::get_tmpFile());
    }

    //auto file = bigCSV::tmpFileFactory::get_tmpFile();


    //run(std::cin);

    return 0;
}
