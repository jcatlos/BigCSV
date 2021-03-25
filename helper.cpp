//
// Created by jakub on 15. 3. 2021.
//

#include <sstream>

#include <iostream>

#include "helper.hpp"

namespace bigCSV{
    std::string trimmedString(const std::string& str){
        std::string::const_iterator it = str.begin();
        while (it != str.end() && isspace(*it)) it++;

        std::string::const_reverse_iterator rit = str.rbegin();
        while (rit.base() != it && isspace(*rit)) rit++;

        return std::string(it, rit.base());
    }

    std::string getQuotedString(std::ifstream& input_stream,
                                        std::string::const_iterator &line_it,
                                        std::string& line,
                                        char quotechar,
                                        char endline)
    {
        std::string out;
        char c = *(++line_it);
        while (!input_stream.eof()) {
            if (c == quotechar) {
                if (++line_it == line.end()) break;         // If the quotechar is at the end of the line, the quoted string ends
                c = *(line_it);                                   // Now c is the char after the quotechar
                if (c == quotechar) out += quotechar;       // Else if c is not quotechar (escaping is done by double quotechar), the quoted string ends
                else break;
            }
            else if (line_it == line.end()) {               // If a endline is encountered while in quoted sequence, the line end is a part of the cell
                out += endline;                             // So a new line is read and used as the current line
                std::getline(input_stream, line, endline);
                line_it = line.begin();
            }
            else {
                out += c;
            }
            c = *(++line_it);                               // Read the next character
        }
        return out;
    }

    std::string formatRow(const std::vector<std::string>& cells, char delimiter, char quotechar, char endline){
        std::string output = "";
        if(cells.empty()) return output;

        output += quotechar + cells[0] + quotechar;
        for(int i = 1; i<cells.size(); i++){
            output += delimiter;
            output += quotechar + cells[i] + quotechar;
        }
        output += endline;

        return output;
    }

    std::vector<std::string> split(const std::string& in, const char delim){
        std::vector<std::string> out;
        std::stringstream stream(in);
        std::string substr;
        while(std::getline(stream, substr, delim)){
            out.push_back(substr);
        }
        return out;
    }

    bool setAttribute(const std::string& pair, std::map<std::string, std::string> attributes){
        auto result = split(pair, '=');
        // Invalid form of attribute setting
        if(result.size() != 2) return false;
        auto key_it = attributes.find(result[0]);
        // Invalid attribute name
        if(key_it == attributes.end()) return false;
        key_it->second = result[1];
        return true;
    }
}