#ifndef BIGCSV_CSVTABLE_HPP
#define BIGCSV_CSVTABLE_HPP

#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

#include "csvFile.hpp"
#include "RowComparator.hpp"

namespace bigCSV{
    class csvTable{
    private:
        // The map ensures that only unique files are in the table
        std::map<std::filesystem::path, csvFile> input_files;

        // Merges the files by a comparator and returns the resulting file
        csvFile merge2(csvFile &first, csvFile &second, const RowComparator &comp) const;
    public:
        // Table attributes
        char in_delimiter;
        char in_quotechar;
        char in_endline;

        char out_delimiter;
        char out_quotechar;
        char out_endline;

        int max_filesize = 13500;

        // Vector of names of all columns in the table
        std::vector<std::string> schema;

        // Constructors - with and without efault values
        csvTable(char delim, char qchar, char endl, int size)
            : in_delimiter(delim), in_quotechar(qchar), in_endline(endl), out_delimiter(delim), out_quotechar(qchar), out_endline(endl), max_filesize(size){}
        csvTable()
            : in_delimiter(','), in_quotechar('"'), in_endline('\n'), out_delimiter(','), out_quotechar('"'), out_endline('\n'){}

        // Prints given columns if they satisfy the provided conditions
        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const Conditions& conditions);

        // Sorts the table by a Comparator and outputs it to given stream.
            // First overload uses all rows and columns
            // Second allows to reduce both the rows and columns - Faster sorting for non-trivially sorted files as less files are required for distribuion
        void sort(std::ostream& out, const RowComparator& comp);
        void sort(std::ostream& out, const RowComparator& comp, const Conditions& conditions, const std::vector<std::string>& columns);

        // Add a file at path to the table with specified delimiter, endline and quotechar
        void addFile(const std::filesystem::path& path, char delimiter, char endline, char quotechar);

        // Uses the RowUpdate on all rows satisfying RowCondition
            // Creates copies of the files to prevent source file modification
        void updateTable(const Conditions &conditions, BigCSV::RowUpdate &update);
    };
}

#endif
