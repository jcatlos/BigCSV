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

    // Opens the input file at the beginning to be read from
    void csvFile::open_input_stream() {
        input_stream.close();                                           // Close the file if it is opened
        input_stream.open(file.get_path(), std::ifstream::in);

        // Initialize the file
        schema = getNextLine();
        for(int i = 0; i < schema.size(); i++){
            columns[trimmedString(schema[i])] = i;
        }
    }

    // Closes the input_file
    void csvFile::close_input_stream() {
        input_stream.close();
    }

    // Retrieve the path of the input file
    std::filesystem::path csvFile::get_path() const {
        return file.get_path();
    }

    // Get the next line as a vector of strings
        // Eats all the leading and trailing whitespace
    std::vector<std::string> csvFile::getNextLine() {
        std::vector<std::string> out = std::vector<std::string>();

        std::string line;
        std::getline(input_stream, line, endline);

        /*if(line.length() == 0){
            std::cerr<<"empty line"<<std::endl;
            return out;
        }*/

        // Eat all of the whitespace before the first token
        std::string::const_iterator line_it = line.begin();
        while(line_it != line.end() && std::isspace(*line_it)) line_it++;

        std::string token = "";
        while (line_it != line.end()) {
            char c = *line_it;
            // Parse quoted strings
            if (c == quotechar) {
                token = "";
                token += getQuotedString(input_stream, line_it, line, quotechar, endline);
                while(line_it != line.end() && std::isspace(*line_it)) line_it++;
                if(line_it == line.end()) break;
                c = *line_it;
            }

            // An end of a token was found - cannot be else if, because the previous branch may have moved the iterator
            if (c == delimiter) {
                out.push_back(token);
                token = "";
                line_it++;
                while(line_it != line.end() && std::isspace(*line_it)) line_it++;
            }

            // Otherwise just add the character to the token
            else {
                token += c;
                line_it++;
            }
        }

        // The line must not end with a in_delimiter, so the last was never added to the line
        out.push_back(token);

        return out;
    }

    // Mask construction
        // Mask = vector of indexes of columns to be printed
        // If a column name is not found in the columns of the file, MAXINT is set and skipped in selection
    std::vector<int> csvFile::create_mask(const std::vector<std::string>& input_columns){
        std::vector<int> line_mask;
        for(const auto& column : input_columns){
            auto trimmed_col = trimmedString(column);
            if(columns.find(trimmed_col) == columns.end()) line_mask.push_back(INT32_MAX);
            else line_mask.push_back(columns[trimmed_col]);
        }
        return line_mask;
    }

    TableRow csvFile::getNextTableRow() {
        TableRow out;
        if(not_eof()){
            auto line = getNextLine();
            //for(const auto& token : line) std::cout<<token<<" || ";
            for(int i=0; i<line.size(); i++){
                //std::cout<<"i = "<<i<<" schema size = "<<schema.size()<<std::endl;
                out.empty = false;
                out.map[schema[i]] = line[i];
                out.schema.push_back(schema[i]);
                out.values.push_back(line[i]);
            }
        }
        return out;
    }

    void csvFile::printColumns(std::ostream& out) {
        printColumns(out, schema);
    }

    void csvFile::printColumns(std::ostream& out, const std::vector<std::string>& input_columns) {
        Conditions conds;
        printColumns(out, input_columns, conds, delimiter, quotechar, endline);
    }

    void csvFile::printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const Conditions& conditions, char out_delimiter, char out_quotechar, char out_endline) {
        open_input_stream();
        std::vector<std::string> line_tokens;
        std::vector<int> line_mask = create_mask(input_columns);
        std::vector<std::string> out_tokens;

        while(not_eof()){
            line_tokens = getNextLine();
            if(!conditions.Hold(line_tokens)) continue;                         // If the line does not pas the condition, skip it
            // Construct the line for output
            out_tokens.clear();
            for(auto&& index : line_mask){
                if(line_tokens.size() <= index) out_tokens.push_back("");       // If index is too high, skip it
                else out_tokens.push_back(line_tokens[index]);
            }
            // Append it to the output
            out<<formatRow(out_tokens, out_delimiter, out_quotechar, out_endline);
        }
        close_input_stream();
    }

    // Sort used on a file small enough to be sorted in memory
    void csvFile::trivialSort(std::ostream& out, const RowComparator& comp){
        // Add all lines of a file in a vector
        std::vector<TableRow> lines;
        open_input_stream();
        while(not_eof()){
            lines.push_back(getNextTableRow());
        }
        close_input_stream();
        // Call the sort function using provided RowComparator
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

    std::vector<csvFile> csvFile::distribute(const Conditions& conditions, std::size_t max_filesize){
        return distribute(conditions, schema, max_filesize);
    }

    std::vector<csvFile> csvFile::distribute(const Conditions& conditions, const std::vector<std::string>& input_columns, std::size_t max_filesize){
        //std::cout<<"calling distribute function"<<std::endl;
        std::vector<csvFile> out;
        std::vector<int> line_mask = create_mask(input_columns);
        std::string header = formatRow(input_columns, delimiter, quotechar, endline);

        open_input_stream();
        while(not_eof()){
            // Create next output file
            auto tmp_file = tmpFileFactory::get_tmpFile();
            std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
            //Add the header row to each file
            out_file<<header;
            std::uintmax_t file_size = 0;
            // Fill it while the main file is not empty or the output file is not full
            while(not_eof() && file_size < max_filesize){      // CHANGE MAX FILE SIZE (for in-memory sort)
                auto line_tokens = getNextLine();
                std::vector<std::string> out_tokens;
                if(!conditions.Hold(line_tokens)) continue;
                for(auto&& index : line_mask){
                    if(line_tokens.size() <= index) out_tokens.push_back("");       // If index is too high, skip it
                    else out_tokens.push_back(line_tokens[index]);
                }
                auto row = formatRow(out_tokens, delimiter, quotechar, endline);
                file_size += row.size();
                out_file<<row;
            }
            // Close the file and add it to the output
            out_file.close();
            out.emplace_back(std::move(tmp_file), delimiter, endline, quotechar);
        }
        close_input_stream();

        std::cout<<"Distribute finished"<<std::endl;

        return out;
    }

    csvFile csvFile::createUpdatedFile(const Conditions &conditions, BigCSV::RowUpdate &update) {
        std::string header = formatRow(schema, delimiter, quotechar, endline);
        auto tmp_file = tmpFileFactory::get_tmpFile();
        std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
        out_file<<header;
        std::vector<std::string> line;

        open_input_stream();
        while(not_eof()){
            line = getNextLine();
            if(conditions.Hold(line)){
                line = update.apply(line);
            }
            out_file<<formatRow(line, delimiter, quotechar, endline);
        }
        close_input_stream();

        return csvFile(std::move(tmp_file), delimiter, endline, quotechar);
    }
}