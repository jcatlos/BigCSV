//
// Created by jakub on 14. 3. 2021.
//

#include "csvTable.hpp"

#include <vector>
#include <fstream>

namespace bigCSV{

    void csvTable::addStream(std::string path, char delimiter, char endline, char quotechar, bool extends) {
        input_files.emplace_back(path, delimiter, endline, quotechar);
        extends_schema.push_back(extends);
    }
}
