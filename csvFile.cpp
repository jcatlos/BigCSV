//
// Created by jakub on 14. 3. 2021.
//

#include "csvFile.hpp"
#include "helper.hpp"
#include "tmpFileFactory.hpp"
#include "Conditions.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

namespace bigCSV {
    csvFile::csvFile(const std::filesystem::path& fn, char delim, char le, char q)
        : delimiter(delim), endline(le), quotechar(q), columns(), file(fn), input_stream()
    {}

    csvFile::csvFile(File&& fn, char delim, char le, char q)
        : delimiter(delim), endline(le), quotechar(q), columns(), file(std::move(fn)), input_stream()
    {}

    void csvFile::initialize() {
        //open_input_stream(false);
        schema = getNextLine();
        column_count = schema.size();
        for(int i = 0; i < schema.size(); i++){
            columns[trimmedString(schema[i])] = i;
        }
        //std::cout<<"Initialized with header "<<formatRow(schema, delimiter,quotechar,endline)<<std::endl;
        //close_input_stream();
    }

    void csvFile::open_input_stream(bool skip_header) {
        input_stream.close();                                           // Close the file if it is opened
        input_stream.open(file.get_path(), std::ifstream::in);
        initialize();
        if(skip_header){                                                // Removing the heading line
            std::string s;
            //std::getline(input_stream, s, endline);
        }
    }

    void csvFile::close_input_stream() {
        input_stream.close();
    }


    std::vector<std::string> csvFile::getNextLine() {
        std::vector<std::string> out = std::vector<std::string>();

        std::string line;
        std::getline(input_stream, line, endline);

        if(line.length() == 0) return out;

        //std::cout<<"line = "<<line<<std::endl;

        // Eat all of the whitespace before the first token
        std::string::const_iterator line_it = line.begin();
        while(std::isspace(*line_it) && line_it != line.end()) line_it++;

        std::string token = "";
        while (line_it != line.end()) {
            char c = *line_it;
            // Parse quoted strings
            if (c == quotechar) {
                token = "";
                token += getQuotedString(*this, line_it, line, quotechar, endline);
                while(std::isspace(*line_it) && line_it != line.end()) line_it++;
                if(line_it == line.end()) break;
                c = *line_it;
            }

            // An end of a token was found - cannot be else if, because the previous branch may have moved the iterator
            if (c == delimiter) {
                out.push_back(token);
                token = "";
                line_it++;
                while(std::isspace(*line_it) && line_it != line.end()) line_it++;
            }

            // Otherwise just add the character to the token
            else {
                token += c;
                line_it++;
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
            out.empty = false;
            out.map[schema[i]] = line[i];
            out.schema.push_back(schema[i]);
            out.values.push_back(line[i]);

        }

        return out;
    }

    void csvFile::printColumns(std::ostream& out) {
        printColumns(out, schema);
    }

    void csvFile::printColumns(std::ostream& out, const std::vector<std::string>& input_columns) {
        printColumns(out, input_columns, tautology, delimiter, quotechar, endline);
    }

    void csvFile::printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const std::function<bool(const std::vector<std::string>&)>& condition, char out_delimiter, char out_quotechar, char out_endline) {
        open_input_stream(true);
        // Printing the first row
        std::vector<std::string> line_tokens;

        // Not needed, pinted in table
        //std::cout<<formatRow(input_columns, delimiter, quotechar, endline);

        std::vector<int> line_mask;
        for(const auto& column : input_columns){
            auto trimmed_col = trimmedString(column);
            //std::cout<<"working with column '"<<column<<"' Changed to '"<<trimmed_col<<"'"<<std::endl;
            if(columns.find(trimmed_col) == columns.end()) line_mask.push_back(INT32_MAX);
            else line_mask.push_back(columns[trimmed_col]);
        }

        std::vector<std::string> out_tokens;
        while(not_eof()){
            line_tokens = getNextLine();
            out_tokens.clear();
            for(auto&& index : line_mask){
                if(line_tokens.size() <= index) out_tokens.push_back("");
                else out_tokens.push_back(line_tokens[index]);
            }
            if(condition(out_tokens)){
                out<<formatRow(out_tokens, out_delimiter, out_quotechar, out_endline);
            }
        }
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
        out<<formatRow(schema, delimiter, quotechar, endline);
        for(auto&& line : lines){
            out<<formatRow(line.toLine(schema), delimiter, quotechar, endline);
        }
    }

    std::vector<csvFile> csvFile::distribute(const std::function<bool(const std::vector<std::string>&)>& condition) {
        std::cout<<"calling distribute function"<<std::endl;
        open_input_stream(true);
        std::vector<csvFile> out;
        std::string header = formatRow(schema, delimiter, quotechar, endline);
        while(not_eof()){
            // Create next output file
            auto tmp_file = tmpFileFactory::get_tmpFile();
            //std::cout<<"file "<<tmp_file.get_path()<<" opened"<<std::endl;
            std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
            //Add the header row to each file
            out_file<<header;
            //std::cout<<header;
            std::uintmax_t file_size = 0;
            // Fill it while the main file is not empty or the output file is not full
            while(not_eof() && file_size < 13500){      // CHANGE MAX FILE SIZE (for in-memory sort)
                auto line = getNextLine();
                if(!condition(line)) continue;
                auto row = formatRow(line, delimiter, quotechar, endline);
                file_size += row.size();
                out_file<<row;
                //std::cout<<row;
            }
            // Close the file and add it to the output
            out_file.close();
            out.emplace_back(std::move(tmp_file), delimiter, endline, quotechar);
        }
        close_input_stream();

        std::cout<<"Distribute finished"<<std::endl;

        return out;
    }
}