//
// Created by jakub on 14. 3. 2021.
//

#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>

#include "file.hpp"
#include "TableRow.hpp"
#include "RowComparator.hpp"

#ifndef BIG_CSV_CSVFILE_HPP
#define BIG_CSV_CSVFILE_HPP

namespace bigCSV {
    class csvFile {
        // As these 3 are defined at the file-level, they have to be stored at each instance of file
        char delimiter;
        char endline;
        char quotechar;
        int column_count;
        std::vector<std::string> col_names;

        void initialize();

    public:
        File file;                      // Move to private when debugged
        std::ifstream input_stream;     // Same

        std::map<std::string, int> columns;

        bool open;

        csvFile(const std::filesystem::path& fn, char delim, char le, char q);
        csvFile(File&& fn, char delim, char le, char q);

        std::vector<std::string> getNextLine();
        TableRow getNextTableRow();

        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns);
        void printColumns(std::ostream& out);
        void trivialSort(std::ostream& out, const RowComparator& comp);
        std::vector<csvFile> distribute();

        std::ifstream& get_ifstream();
        void open_input_stream(bool skip_header);

        void close_input_stream();
    };
}

#endif //BIG_CSV_CSVFILE_HPP
