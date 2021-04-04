//
// Created by jakub on 14. 3. 2021.
//

#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <functional>

#include "file.hpp"
#include "TableRow.hpp"
#include "RowComparator.hpp"
#include "RowUpdate.hpp"
#include "Conditions.hpp"

#ifndef BIG_CSV_CSVFILE_HPP
#define BIG_CSV_CSVFILE_HPP

namespace bigCSV {
    class csvTable;

    class csvFile {
        // As these 3 are defined at the file-level, they have to be stored at each instance of file
        char delimiter;
        char endline;
        char quotechar;

        friend csvTable;

        File file;                      // Move to private when debugged
        std::ifstream input_stream;     // Same

    public:
        std::filesystem::path get_path() const;

        std::map<std::string, int> columns;
        std::vector<std::string> schema;

        inline void init_file() {
            open_input_stream();
            close_input_stream();
        }

        inline bool not_eof() {
            return input_stream.good() && input_stream.peek() != EOF;
        }

        csvFile(const std::filesystem::path& fn, char delim, char le, char q);
        csvFile(File&& fn, char delim, char le, char q);

        std::vector<std::string> getNextLine() ;
        TableRow getNextTableRow() ;

        csvFile createUpdatedFile(const Conditions& conditions, BigCSV::RowUpdate& update) ;

        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const Conditions& conditions, char out_delimiter, char out_quotechar, char out_endline);
        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns);
        void printColumns(std::ostream& out);

        void trivialSort(std::ostream& out, const RowComparator& comp);
        std::vector<csvFile> distribute(const Conditions& condition, std::size_t max_filesize);

        void open_input_stream() ;
        void close_input_stream() ;
    };
}

#endif //BIG_CSV_CSVFILE_HPP
