//
// Created by jakub on 15. 3. 2021.
//

#include <sstream>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cstdio>
#include <set>

#include "helper.hpp"
#include "csvFile.hpp"
#include "tmpFileFactory.hpp"
#include "RowComparator.hpp"

namespace bigCSV{
    std::string trimmedString(const std::string& str){
        std::string::const_iterator it = str.begin();
        while (it != str.end() && isspace(*it)) it++;

        std::string::const_reverse_iterator rit = str.rbegin();
        while (rit.base() != it && isspace(*rit)) rit++;

        return std::string(it, rit.base());
    }

    std::string getQuotedString(bigCSV::csvFile& file,
                                std::string::const_iterator &line_it,
                                std::string& line,
                                char quotechar,
                                char endline)
    {
        std::string out;
        char c = *(++line_it);
        //std::cout<<"\t entered getQuotedString"<<std::endl;
        while(true) {
            //std::cout<<"\t qc = "<<c<<std::endl;
            if (c == quotechar) {
                if (++line_it == line.end()) break;         // If the quotechar is at the end of the line, the quoted string ends
                c = *(line_it);                                   // Now c is the char after the quotechar
                if (c == quotechar) out += quotechar;       // Else if c is not quotechar (escaping is done by double quotechar), the quoted string ends
                else break;
            }
            else if (line_it == line.end()) {               // If a endline is encountered while in quoted sequence, the line end is a part of the cell
                out += endline;                             // So a new line is read and used as the current line
                if(!file.not_eof()) break;
                std::getline(file.input_stream, line, endline);
                line_it = line.begin();
            }
            else {
                out += c;
            }
            c = *(++line_it);                               // Read the next character
        }
        //std::cout<<"\t returning "<<out<<std::endl;
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

    bool setAttribute(const std::string& pair, std::map<std::string, char>& attributes){
        auto result = split(pair, '=');
        // Invalid form of attribute setting
        if(result.size() != 2) return false;
        auto key_it = attributes.find(result[0]);
        // Invalid attribute name
        if(key_it == attributes.end()) return false;
        key_it->second = result[1][0];
        return true;
    }

    std::string getNextWord(std::string& line, std::string::iterator& it){
        std::string out = "";
        while(it != line.end() && isspace(*it)) it++;        // Take all the whitespace before the next word
        bool quoted = *it == '"';
        if(quoted){
            it++;
            while(it != line.end() && *it != '"'){
                out += *it;
                it++;
            }
            it++;
        }
        else{
            while(it != line.end() && !isspace(*it)){
                out += *it;
                it++;
            }
        }
        return out;
    }

    std::vector<std::string> createJoinedSchema (const std::vector<std::string>& first,
                                                 const std::vector<std::string>& second){
        std::vector<std::string> out;
        std::set<std::string> present;

        // Insert all of the columns of the first schema
        for(auto&& col : first){
            out.push_back(col);
            present.insert(col);
        }

        // Insert all columns from the second schema that were no already inserted
        for(auto&& col : second){
            if(present.find(col) == present.end()){
                out.push_back(col);
            }
        }

        return out;
    }

    int index_of(std::string str, const std::vector<std::string> &arr) {
        for(int i=0; i<arr.size(); i++){
            if(arr[i] == str) return i;
        }
        return -1;
    }

    /*bigCSV::csvFile merge(std::vector<bigCSV::csvFile>& input_files){

    }*/
}