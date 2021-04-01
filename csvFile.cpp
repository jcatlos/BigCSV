//
// Created by jakub on 14. 3. 2021.
//

#include "csvFile.hpp"
#include "helper.hpp"
#include "tmpFileFactory.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

namespace bigCSV {
    csvFile::csvFile(const std::filesystem::path& fn, char delim, char le, char q)
            : delimiter(delim), endline(le), quotechar(q), columns(), file(fn), input_stream()
    {
        initialize();
    }

    csvFile::csvFile(File&& fn, char delim, char le, char q)
            : delimiter(delim), endline(le), quotechar(q), columns(), file(std::move(fn)), input_stream()
    {
        initialize();
    }

    void csvFile::initialize() {
        open_input_stream(false);
        col_names = getNextLine();
        column_count = col_names.size();
        for(int i = 0; i<col_names.size(); i++){
            columns[trimmedString(col_names[i])] = i;
        }
        close_input_stream();
    }

    void csvFile::open_input_stream(bool skip_header) {
        input_stream.close();                                           // Close the file if it is opened
        input_stream.open(file.get_path(), std::ifstream::in);
        if(skip_header){                                                // Removing the heading line
            std::string s;
            std::getline(input_stream, s, endline);
        }
    }

    void csvFile::close_input_stream() {
        input_stream.close();
    }


    std::vector<std::string> csvFile::getNextLine() {
        std::vector<std::string> out = std::vector<std::string>();

        std::string line;
        std::getline(input_stream, line, endline);

        /*
        // Check if something can be read from the file
            // Maybe do it before the getline?
        if(input_stream.peek() == EOF){
            open = false;
            return out;
        }
         */
        //if (line.length() == 0) return out;

        std::string token = "";
        for (std::string::const_iterator line_it = line.begin(); line_it != line.end(); line_it++) {
            char c = *line_it;
            // Parse quoted strings
            if (c == quotechar) {
                token = "";
                token += getQuotedString(*this, line_it, line, quotechar, endline);
                c = *line_it;
                if(line_it == line.end()) break;
            }

            // An end of a token was found - cannot be else if, because the previous branch may have moved the iterator
            if (c == delimiter) {
                out.push_back(token);
                token = "";
            }

            // Otherwise just add the character to the token
            else {
                token += c;
            }
        }

        // The line must not end with a delimiter, so the last token must be added into the output
        out.push_back(token);
        
        return out;
    }

    TableRow csvFile::getNextTableRow() {
        auto line = getNextLine();
        TableRow out;
        for(int i=0; i<line.size(); i++){
            out.map[col_names[i]] = line[i];
            out.schema.push_back(col_names[i]);
            out.values.push_back(line[i]);

        }

        return out;
    }

    void csvFile::printColumns(std::ostream& out) {
        printColumns(out, col_names);
    }

    void csvFile::printColumns(std::ostream& out, const std::vector<std::string>& input_columns) {
        open_input_stream(true);
        // Printing the first row
        std::vector<std::string> line_tokens;

        // Not needed, pinted in table
        //std::cout<<formatRow(input_columns, delimiter, quotechar, endline);

        std::vector<int> line_mask;
        for(const auto& column : input_columns){
            std::cout<<"working with column "<<column<<std::endl;
            if(columns.find(column) == columns.end()) line_mask.push_back(INT32_MAX);
            else line_mask.push_back(columns[column]);
        }

        for(auto&& m : line_mask) out<<m<<std::endl;
        std::cout<<"Mask finished"<<std::endl;


        std::vector<std::string> out_tokens;
        while(not_eof()){
            //out<<"i live"<<std::endl;
            line_tokens = getNextLine();
            out_tokens.clear();
            for(auto&& index : line_mask){
                if(line_tokens.size() <= index) out_tokens.push_back("");
                else out_tokens.push_back(line_tokens[index]);
            }
            out<<formatRow(out_tokens, delimiter, quotechar, endline);
        }
        std::cout<<"printed"<<std::endl;
        close_input_stream();
    }

    // Sort used on a file small enough to be sorted in memory
    void csvFile::trivialSort(std::ostream& out, const RowComparator& comp){
        // Add all lines of a file in a vector
        std::vector<TableRow> lines;
        open_input_stream(true);
        while(not_eof()){
            lines.push_back(getNextTableRow());
        }
        close_input_stream();
        // Get indexes of the columns we sort by (in order of sorting)
        // Call the sort function using a custom lambda based on the sorted columns
        std::sort(
                lines.begin(),
                lines.end(),
                comp
        );
        // Now export the lines into a new file
            // for now print it
        for(auto&& line : lines){
            out<<formatRow(line.toLine(col_names), delimiter, quotechar, endline);
        }
    }

    std::vector<csvFile> csvFile::distribute() {
        open_input_stream(true);
        std::vector<csvFile> out;
        std::string header = formatRow(col_names, delimiter, quotechar, endline);
        while(not_eof()){
            // Create next output file
            auto tmp_file = tmpFileFactory::get_tmpFile();
            std::cout<<"file "<<tmp_file.get_path()<<" opened"<<std::endl;
            std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
            //Add the header row to each file
            out_file<<header;
            std::cout<<header;
            std::uintmax_t file_size = 0;
            // Fill it while the main file is not empty or the output file is not full
            while(input_stream.good() && file_size < 1000000){      // CHANGE MAX FILE SIZE (for in-memory sort)
                auto row = formatRow(getNextLine(), delimiter, quotechar, endline);
                file_size += row.size();
                out_file<<row;
                std::cout<<row;
            }
            // Close the file and add it to the output
            out_file.close();
            out.emplace_back(std::move(tmp_file), delimiter, endline, quotechar);
        }
        close_input_stream();
        return out;
    }
}