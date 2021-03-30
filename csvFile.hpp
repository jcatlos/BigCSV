//
// Created by jakub on 14. 3. 2021.
//

#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>

#include "file.hpp"

#ifndef BIG_CSV_CSVFILE_HPP
#define BIG_CSV_CSVFILE_HPP

namespace bigCSV {
    class csvFile {
        std::ifstream input_stream;
        // As these 3 are defined at the file-level, they have to be stored at each instance of file
        char delimiter;
        char endline;
        char quotechar;
        int column_count;
        std::vector<std::string> col_names;

        File file;


        void initialize();
        void resetFile();

    public:
        std::map<std::string, int> columns;

        csvFile(const std::filesystem::path& fn, char delim, char le, char q);
        csvFile(File&& fn, char delim, char le, char q);

        std::vector<std::string> getNextLine();
        void printColumns(std::vector<std::string> input_columns);
        void trivialSort(std::vector<std::string> sortColumns);
        std::vector<csvFile> distribute();
    };
}

#endif //BIG_CSV_CSVFILE_HPP
