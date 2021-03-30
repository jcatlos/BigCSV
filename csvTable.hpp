//
// Created by jakub on 14. 3. 2021.
//

#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

#include "csvFile.hpp"

#ifndef BIG_CSV_CSVTABLE_HPP

#define BIG_CSV_CSVTABLE_HPP

namespace bigCSV{
    class csvTable{

        char delimiter;
        char quotechar;
        char endline;

        std::map<std::filesystem::path, csvFile> input_files;
        //std::map<std::string, int> columns;

    public:


        csvTable(char delim, char qchar, char endl)
            : delimiter(delim), quotechar(qchar), endline(endl){}
        csvTable()
            : delimiter(','), quotechar('"'), endline('\n'){}

        void printColumns(const std::vector<std::string>& input_columns);// IMPLEMENT

        void sort(std::ofstream out, std::vector<std::string> sortColumns); // IMPLEMENT

        void addStream(std::filesystem::path path, char delimiter, char endline, char quotechar, bool extends);
    };
}


#endif //BIG_CSV_CSVTABLE_HPP
