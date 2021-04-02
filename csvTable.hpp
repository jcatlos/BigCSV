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

        char delimiter;
        char quotechar;
        char endline;

        std::map<std::filesystem::path, csvFile> input_files;
        //std::map<std::string, int> columns;

    public:

        std::vector<std::string> schema;

        csvTable(char delim, char qchar, char endl)
            : delimiter(delim), quotechar(qchar), endline(endl){}
        csvTable()
            : delimiter(','), quotechar('"'), endline('\n'){}

        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const std::function<bool(const std::vector<std::string>&)>& condition);

        void sort(std::ostream& out, const RowComparator& comp);
        void sort(std::ostream& out, const RowComparator& comp, const std::function<bool(const std::vector<std::string>&)>& condition);

        void addStream(const std::filesystem::path& path, char delimiter, char endline, char quotechar);

        csvFile merge2(csvFile &first, csvFile &second, const RowComparator &comp) const;
    };
}


#endif //BIG_CSV_CSVTABLE_HPP
