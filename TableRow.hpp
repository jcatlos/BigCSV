//
// Created by jakub on 31. 3. 2021.
//

#ifndef CATLOS_TABLEROW_HPP
#define CATLOS_TABLEROW_HPP

#include <map>
#include <string>
#include <vector>

namespace bigCSV{
    struct TableRow {
        std::vector<std::string> schema;
        std::vector<std::string> values;
        std::map<std::string, std::string> map;

        bool empty = true;

        void add(const std::string& col_name, const std::string& value);
        //void add(std::string&& col_name, std::string&& value);
        std::vector<std::string> toLine(const std::vector<std::string> &input_schema);
    };
}

#endif //CATLOS_TABLEROW_HPP
