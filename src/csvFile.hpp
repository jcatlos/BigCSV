#ifndef BIGCSV_CSVFILE_HPP
#define BIGCSV_CSVFILE_HPP

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

namespace bigCSV {
    class csvTable;

    class csvFile {
    private:
        friend csvTable;

        // As these 3 are defined at the file-level, they have to be stored at each instance of file
        char delimiter;
        char endline;
        char quotechar;

        // The source file
        File file;        
        // The internal ifstream for working with the file              
        std::ifstream input_stream;     

        // Given the column names, returns a vector of ints of the same length
            // If a column with such name is in the file, its index in `schema` is on the position
            // Otherwise an arbitrarily high number
        std::vector<int> create_mask(const std::vector<std::string> &input_columns);


    public:
        // Constructors:
            // First creates a File instance from path, second takes a rvalue reference
        csvFile(const std::filesystem::path& fn, char delim, char le, char q);
        csvFile(File&& fn, char delim, char le, char q);

        // Returns the path of the source file
        std::filesystem::path get_path() const;

        // Data about the column names and positions
        std::map<std::string, int> columns;
        std::vector<std::string> schema;

        inline void init_file() {
            open_input_stream();
            close_input_stream();
        }

        inline bool not_eof() {
            return input_stream.good() && input_stream.peek() != EOF;
        }

        // Functions for reading rows in different formats
        std::vector<std::string> getNextLine() ;        // vector of strings in the order in the source file
        TableRow getNextTableRow() ;                    // a TableRow 

        // Retrns a copy of the file with updated rows
            // Calls the RowUpdate on all rows satisfying the Conditions
        csvFile createUpdatedFile(const Conditions& conditions, BigCSV::RowUpdate& update) ;

        // Print columns to the output
            // Overloads set default values to attributes
        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const Conditions& conditions, char out_delimiter, char out_quotechar, char out_endline);
        void printColumns(std::ostream& out, const std::vector<std::string>& input_columns);
        void printColumns(std::ostream& out);

        void trivialSort(std::ostream& out, const RowComparator& comp);

        // These functions perform the opening and closing of the input stream
            // On opening, the initialization of the fil's schema is done
            // Should be called around any reading operations form the file
        void open_input_stream();
        void close_input_stream();

        // Distributes the file into files of max_filesize
            // Copies only rows satisfying the Conditions
            // Copies only the selected columns
            // These reduce the file sizes, so the sort is much quicker
            // The second overload just copies all rows and columns
        std::vector<csvFile> distribute(const Conditions &conditions, const std::vector<std::string> &input_columns, size_t max_filesize);
        std::vector<csvFile> distribute(const Conditions& condition, std::size_t max_filesize);
    };
}

#endif 
