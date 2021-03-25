//
// Created by jakub on 14. 3. 2021.
//

#include <vector>
#include <fstream>
#include <string>

#include "csvFile.hpp"

#ifndef BIG_CSV_CSVTABLE_HPP

#define BIG_CSV_CSVTABLE_HPP

namespace bigCSV{
    class csvTable{

        char delimiter;
        char quotechar;
        char endline;

        std::vector<std::string> columns;
        std::vector<csvFile> input_files;
        std::vector<bool> extends_schema;           // extends_schema[i] is true when


        void addStream(std::string path, char delimiter, char endline, char quotechar, bool extends);


    public:
        csvTable(char delim, char qchar, char endl) : delimiter(delim), quotechar(qchar), endline(endl), columns(), input_files(){}
        csvTable() : delimiter(','), quotechar('"'), endline('\n'), columns(), input_files(){}
    };
}


#endif //BIG_CSV_CSVTABLE_HPP
