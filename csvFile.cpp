//
// Created by jakub on 14. 3. 2021.
//

#include "csvFile.hpp"
#include "helper.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

namespace bigCSV {
    csvFile::csvFile(const std::string& fn, char delim, char le, char q)
            : delimiter(delim), endline(le), quotechar(q), columns(), file_name(fn)
        {
            // Find the last dot in the file and take everything before as the name
            for(int dot_i = fn.length()-1; dot_i >= 0; dot_i--){

                // IMPLEMENT

            }
            input_stream.open(file_name, std::ifstream::in);
            auto col_names = this->getNextLine();
            column_count = col_names.size();
            for(int i = 0; i<col_names.size(); i++){
                columns[trimmedString(col_names[i])] = i;
            }
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

}