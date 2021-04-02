#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "csvFile.hpp"
#include "csvTable.hpp"
#include "tmpFileFactory.hpp"
#include "file.hpp"
#include "helper.hpp"
#include "Conditions.hpp"
#include "shell.hpp"

int main() {

    bigCSV::run(std::cin);
    //std::cout<<std::filesystem::temp_directory_path()<<std::endl;
    /*bigCSV::csvTable table;
    table.addStream("input2.csv", ',', '\n', '"');

    std::vector<std::string> printed_columns;
    printed_columns.push_back("NazevZarizeni");
    printed_columns.push_back("Obec");
    //printed_columns.push_back("First name");

    //std::cout<<"sorting table"<<std::endl;
    auto cond = bigCSV::create_equal_check(5, "Praha 2");
    table.sort(std::cout, printed_columns, cond);

    //table.printColumns(std::cout, printed_columns, bigCSV::tautology);

    //bigCSV::csvFile file("input.csv", ',', '\n', '"');
    //std::cout<<"PRINTING COLUMNS"<<std::endl;
    //table.printColumns(std::cout, printed_columns);
    //file.trivialSort(printed_columns);*/

/*
    auto files = file.distribute();
    //std::cout<<" size: "<<files.size()<<std::endl;
    files[2].printColumns(std::cout, printed_columns);



*/
    //auto file = bigCSV::tmpFileFactory::get_tmpFile();


    //run(std::cin);

    //std::cout<<"0 open: "<<files[0].input_stream.is_open()<<std::endl;

    std::cout<<"END OF MAIN"<<std::endl;
    return 0;
}
