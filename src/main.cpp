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

    std::cout<<"BigCSV Shell: Program for manipulating large .csv files"<<std::endl;
    std::cout<<"For instructions, type in 'HELP;' or read the user documentation"<<std::endl;

    bigCSV::Shell shell(std::cin, std::cerr);
    shell.run();
    
    return 0;
}
