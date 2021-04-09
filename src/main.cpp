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

    bigCSV::Shell shell(std::cin, std::cerr);
    shell.run();
    
    return 0;
}
