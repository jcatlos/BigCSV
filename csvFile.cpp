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
            : delimiter(delim), endline(le), quotechar(q), columns(), file(fn), input_stream(fn, std::ifstream::in)
    {
        initialize();
    }

    csvFile::csvFile(File&& fn, char delim, char le, char q)
            : delimiter(delim), endline(le), quotechar(q), columns(), file(std::move(fn)), input_stream(fn.get_path(), std::ifstream::in)
    {
        initialize();
    }

    void csvFile::initialize() {
        col_names = this->getNextLine();
        column_count = col_names.size();
        for(int i = 0; i<col_names.size(); i++){
            columns[trimmedString(col_names[i])] = i;
        }
    }

    void csvFile::resetFile(){
        input_stream.close();
        input_stream.open(file->get_path(), std::ifstream::in);
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

        // The line must not end with a delimiter, so the last token must be added into the output
        out.push_back(token);

        return out;
    }

    void csvFile::printColumns(std::vector<std::string> input_columns) {
        // Printing the first row
        std::vector<std::string> line_tokens;
        for(auto col_it = input_columns.begin(); col_it != input_columns.end(); col_it++){
            line_tokens.push_back(*col_it);
        }
        std::cout<<formatRow(line_tokens, delimiter, quotechar, endline);

        std::vector<int> line_mask;
        for(auto&& column : input_columns){
            line_mask.push_back(columns[column]);
        }
        for(auto&& index : line_mask){
            std::cout<<index<<std::endl;
        }

        resetFile();
        std::vector<std::string> out_tokens;
        while(input_stream.good()){
            line_tokens = getNextLine();
            out_tokens.clear();
            for(auto&& index : line_mask){
                out_tokens.push_back(line_tokens[index]);
            }
            std::cout<<formatRow(out_tokens, delimiter, quotechar, endline);
        }
    }

    // Sort used on a file small enough to be sorted in memory
    void csvFile::trivialSort(std::vector<std::string> sortColumns){
        // Add all lines of a file in a vector
        std::vector<std::vector<std::string>> lines;
        resetFile();
        while(input_stream.good()){
            lines.push_back(getNextLine());
        }
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
        std::vector<csvFile> out;
        std::string header = formatRow(col_names, delimiter, quotechar, endline);
        resetFile();
        while(input_stream.good()){
            // Create next output file
            auto tmp_file = tmpFileFactory::get_tmpFile();
            //std::cout<<"file "<<out_path.filename()<<" opened"<<std::endl;
            std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
            //Add the header row to each file
            out_file<<header;
            // Fill it while the main file is not empty or the output file is not full
            while(input_stream.good() && std::filesystem::file_size(tmp_file.get_path()) < 1000){      // CHANGE MAX FILE SIZE (for in-memory sort)
                out_file<<formatRow(getNextLine(), delimiter, quotechar, endline);
            }
            // Close the file and add it to the output
            out.emplace_back(std::move(tmp_file), delimiter, endline, quotechar);
            std::cout<<"emplaced"<<std::endl;
        }
        return out;
    }
}