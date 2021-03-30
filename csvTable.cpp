//
// Created by jakub on 14. 3. 2021.
//

#include "csvTable.hpp"
#include "helper.hpp"

#include <vector>
#include <fstream>
#include <iostream>

namespace bigCSV{

    // REMOVE THE EXTENDS?
    void csvTable::addStream(std::filesystem::path path, char delimiter, char endline, char quotechar, bool extends) {
        csvFile file(path, delimiter, endline, quotechar);
        input_files.emplace(path, std::move(file));
    }

    // FOR NOW ONLY WORKS ON CONSOLE OUTPUT
    void csvTable::printColumns(const std::vector<std::string>& input_columns){
        std::cout<<formatRow(input_columns, delimiter, quotechar, endline);
        for(auto&& file: input_files){
            file.second.printColumns(input_columns);
        }
    }

}
