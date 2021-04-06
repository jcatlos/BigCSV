//
// Created by jakub on 14. 3. 2021.
//

#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

#include "csvFile.hpp"
#include "RowComparator.hpp"

#ifndef BIG_CSV_CSVTABLE_HPP
#define BIG_CSV_CSVTABLE_HPP

namespace bigCSV{
    class csvTable{
    private:
        std::map<std::filesystem::path, csvFile> input_files;

    public:

        char in_delimiter;
        char in_quotechar;
        char in_endline;

        char out_delimiter;
        char out_quotechar;
        char out_endline;

        std::size_t max_filesize = 13500;

        std::vector<std::string> schema;

        csvTable(char delim, char qchar, char endl)
            : in_delimiter(delim), in_quotechar(qchar), in_endline(endl), out_delimiter(delim), out_quotechar(qchar), out_endline(endl){}
        csvTable()
            : in_delimiter(','), in_quotechar('"'), in_endline('\n'), out_delimiter(','), out_quotechar('"'), out_endline('\n'){}

        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const Conditions& conditions);

        void sort(std::ostream& out, const RowComparator& comp);
        void sort(std::ostream& out, const RowComparator& comp, const Conditions& conditions, const std::vector<std::string>& columns);

        void addFile(const std::filesystem::path& path, char delimiter, char endline, char quotechar);

        void updateTable(const Conditions &conditions, BigCSV::RowUpdate &update);

        csvFile merge2(csvFile &first, csvFile &second, const RowComparator &comp) const;
    };
}


#endif //BIG_CSV_CSVTABLE_HPP
