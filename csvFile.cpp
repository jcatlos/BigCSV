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
            : delimiter(delim), endline(le), quotechar(q), columns(), file(fn), input_stream(), open(false)
    {
        initialize();
    }

    csvFile::csvFile(File&& fn, char delim, char le, char q)
            : delimiter(delim), endline(le), quotechar(q), columns(), file(std::move(fn)), input_stream(), open(false)
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
        open = true;
        if(skip_header){                                                // Removing the heading line
            std::string s;
            std::getline(input_stream, s, endline);
        }
    }

    void csvFile::close_input_stream() {
        input_stream.close();
        open = false;
    }


    std::vector<std::string> csvFile::getNextLine() {
        std::vector<std::string> out = std::vector<std::string>();

        std::string line;
        std::getline(input_stream, line, endline);
        if (line.length() == 0) return out;

        std::string token = "";
        for (std::string::const_iterator line_it = line.begin(); line_it != line.end(); line_it++) {
            char c = *line_it;

            // Parse quoted strings
            if (c == quotechar) {
                token = "";
                token += getQuotedString(input_stream, line_it, line, quotechar, endline);
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

        // If !input_stream.good() set the open bit to false
        open = input_stream.good();

        // The line must not end with a delimiter, so the last token must be added into the output
        out.push_back(token);

        return out;
    }

    std::map<std::string, std::string> csvFile::getNextLineAsMap(){
        auto line = getNextLine();
        std::map<std::string, std::string> out;
        for(auto&& col: columns){
            out[col.first] = line[col.second];
        }
        return out;
    }

    void csvFile::printColumns(std::vector<std::string> input_columns) {
        open_input_stream(true);
        // Printing the first row
        std::vector<std::string> line_tokens;
        /* WTF IS THIS??
         * for(auto col_it = input_columns.begin(); col_it != input_columns.end(); col_it++){
            line_tokens.push_back(*col_it);
        }*/

        // Not needed, pinted in table
        //std::cout<<formatRow(input_columns, delimiter, quotechar, endline);

        std::vector<int> line_mask;
        for(auto&& column : input_columns){
            if(columns.find(column) == columns.end()) line_mask.push_back(INT32_MAX);
            else line_mask.push_back(columns[column]);
        }

        std::vector<std::string> out_tokens;
        while(input_stream.good()){
            line_tokens = getNextLine();
            out_tokens.clear();
            for(auto&& index : line_mask){
                if(line_tokens.size() <= index) out_tokens.push_back("");
                else out_tokens.push_back(line_tokens[index]);
            }
            std::cout<<formatRow(out_tokens, delimiter, quotechar, endline);
        }
        close_input_stream();
    }

    // Sort used on a file small enough to be sorted in memory
    void csvFile::trivialSort(std::vector<std::string> sortColumns){
        // Add all lines of a file in a vector
        std::vector<std::vector<std::string>> lines;
        open_input_stream(true);
        while(input_stream.good()){
            lines.push_back(getNextLine());
        }
        close_input_stream();
        // Get indexes of the columns we sort by (in order of sorting)
        std::vector<int> mask;
        for(auto&& col : sortColumns){
            mask.push_back(columns[col]);
        }
        // Call the sort function using a custom lambda based on the sorted columns
        std::sort(
                lines.begin(),
                lines.end(),
                [&mask](const std::vector<std::string>& a, const std::vector<std::string>& b)
                    {
                        for(const auto& m : mask){
                            if(a[m].compare(b[m]) < 0) return true;
                            if(a[m].compare(b[m]) > 0) return false;
                        }
                        return true;
                    }
        );
        // Now export the lines into a new file
            // for now print it
        for(auto&& line : lines){
            std::cout<<formatRow(line, delimiter, quotechar, endline);
        }
    }

    std::vector<csvFile> csvFile::distribute() {
        open_input_stream(true);
        std::vector<csvFile> out;
        std::string header = formatRow(col_names, delimiter, quotechar, endline);
        while(input_stream.good()){
            // Create next output file
            auto tmp_file = tmpFileFactory::get_tmpFile();
            //std::cout<<"file "<<out_path.filename()<<" opened"<<std::endl;
            std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
            //Add the header row to each file
            out_file<<header;
            std::uintmax_t file_size = 0;
            // Fill it while the main file is not empty or the output file is not full
            while(input_stream.good() && file_size < 1000000){      // CHANGE MAX FILE SIZE (for in-memory sort)
                auto row = formatRow(getNextLine(), delimiter, quotechar, endline);
                file_size += row.size();
                out_file<<row;
            }
            // Close the file and add it to the output
            out_file.close();
            out.emplace_back(std::move(tmp_file), delimiter, endline, quotechar);
        }
        close_input_stream();
        return out;
    }
}