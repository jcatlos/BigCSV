//
// Created by jakub on 15. 3. 2021.
//

#include <iostream>
#include <random>
#include <set>

#include "helper.hpp"
#include "csvFile.hpp"


namespace bigCSV{

    // Returns a copy of a string without leading and trailing spaces
    std::string trimmedString(const std::string& str){
        std::string::const_iterator it = str.begin();
        while (it != str.end() && isspace(*it)) it++;

        std::string::const_reverse_iterator rit = str.rbegin();
        while (rit.base() != it && isspace(*rit)) rit++;

        return std::string(it, rit.base());
    }

    // Parses a quoted string from the input according to the RFC4180 CSV file standard
        // A quotation character can be escaped only by another quotation character
            // "quotechar is: ""." -> quotechar is: ".
        // Endline character in quotes is a part of the string
        // If the input ends before the second quotechar appears, the string is returned as if there was a quotechar at the end of the file (benevolent)
    std::string getQuotedString(std::istream& in,
                                std::string::const_iterator &line_it,
                                std::string& line,
                                char quotechar,
                                char endline)
    {
        std::string out;
        char c = *(++line_it);
        while(true) {
            if (c == quotechar) {
                if (++line_it == line.end()) break;         // If the in_quotechar is at the end of the line, the quoted string ends
                c = *(line_it);                                   // Now c is the char after the in_quotechar
                if (c == quotechar) out += quotechar;       // Else if c is not in_quotechar (escaping is done by double in_quotechar), the quoted string ends
                else break;
            }
            else if (line_it == line.end()) {               // If a in_endline is encountered while in quoted sequence, the line end is a part of the cell
                out += endline;                             // So a new line is read and used as the current line
                //if(!file.not_eof()) break;
                std::getline(in, line, endline);
                if(line.empty()) break;
                line_it = line.begin();
            }
            else {
                out += c;
            }
            c = *(++line_it);                               // Read the next character
        }

        return out;
    }

    // Given a vector of fields and the appropriate characters, constructs a proper, quoted row for a csv table
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

    // Returns a vector of tokens from the input string divided by a delimiter
    std::vector<std::string> split(const std::string& in, const char delim){
        std::vector<std::string> out;
        std::stringstream stream(in);
        std::string substr;
        while(std::getline(stream, substr, delim)){
            out.push_back(substr);
        }
        return out;
    }

    // Given 2 row schemas, returns a schema containing all columns from the two
        // Works like set union
    std::vector<std::string> createJoinedSchema (const std::vector<std::string>& first,
                                                 const std::vector<std::string>& second){
        std::vector<std::string> out;
        std::set<std::string> present;

        // Insert all of the columns of the first schema
        for(const auto& col : first){
            out.push_back(col);
            present.insert(col);
        }

        // Insert all columns from the second schema that were no already inserted
        for(const auto& col : second){
            if(present.find(col) == present.end()){
                out.push_back(col);
            }
        }

        return out;
    }

    // Returns an index of a string in a vector of strings
        // If not found, return -1
    int index_of(std::string str, const std::vector<std::string> &arr) {
        for(int i=0; i<arr.size(); i++){
            if(arr[i] == str) return i;
        }
        return -1;
    }

}