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

    void csvFile::open_input_stream() {
        input_stream.close();                                           // Close the file if it is opened
        input_stream.open(file.get_path(), std::ifstream::in);

        // Initialize the file
        schema = getNextLine();
        for(int i = 0; i < schema.size(); i++){
            columns[trimmedString(schema[i])] = i;
        }
    }

    void csvFile::close_input_stream() {
        input_stream.close();
    }

    std::filesystem::path csvFile::get_path() const {
        return file.get_path();
    }

    std::vector<std::string> csvFile::getNextLine() {
        std::vector<std::string> out = std::vector<std::string>();

        std::string line;
        std::getline(input_stream, line, endline);

        if(line.length() == 0) return out;

        // Eat all of the whitespace before the first token
        std::string::const_iterator line_it = line.begin();
        while(std::isspace(*line_it) && line_it != line.end()) line_it++;

        std::string token = "";
        while (line_it != line.end()) {
            char c = *line_it;
            // Parse quoted strings
            if (c == quotechar) {
                token = "";
                token += getQuotedString(input_stream, line_it, line, quotechar, endline);
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

        // The line must not end with a in_delimiter, so the last token must be added into the output
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
        open_input_stream();
        std::vector<std::string> line_tokens;

        // Mask construction
            // Mask = vector of indexes of columns to be printed
            // If a column name is not found in the columns of the file, MAXINT is set and skipped in selection
        std::vector<int> line_mask;
        for(const auto& column : input_columns){
            auto trimmed_col = trimmedString(column);
            if(columns.find(trimmed_col) == columns.end()) line_mask.push_back(INT32_MAX);
            else line_mask.push_back(columns[trimmed_col]);
        }

        // Printing columns
        std::vector<std::string> out_tokens;
        while(not_eof()){
            // Construct the line
            line_tokens = getNextLine();
            out_tokens.clear();
            for(auto&& index : line_mask){
                if(line_tokens.size() <= index) out_tokens.push_back("");       // If index is too high, skip it
                else out_tokens.push_back(line_tokens[index]);
            }
            // Print if it satisfies the condition
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
        open_input_stream();
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

    std::vector<csvFile> csvFile::distribute(const std::function<bool(const std::vector<std::string>&)>& condition){
        std::cout<<"calling distribute function"<<std::endl;
        std::vector<csvFile> out;
        std::string header = formatRow(schema, delimiter, quotechar, endline);

        open_input_stream();
        while(not_eof()){
            // Create next output file
            auto tmp_file = tmpFileFactory::get_tmpFile();
            //std::cout<<"file "<<tmp_file.get_path()<<" opened"<<std::endl;
            std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
            //Add the header row to each file
            out_file<<header;
            std::uintmax_t file_size = header.size();
            // Fill it while the main file is not empty or the output file is not full
            while(not_eof() && file_size < 13500){      // CHANGE MAX FILE SIZE (for in-memory sort)
                auto line = getNextLine();
                if(!condition(line)) continue;
                auto row = formatRow(line, delimiter, quotechar, endline);
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

    csvFile csvFile::createUpdatedFile(const std::function<bool(const std::vector<std::string> &)> &condition, BigCSV::RowUpdate &update) {
        std::string header = formatRow(schema, delimiter, quotechar, endline);
        auto tmp_file = tmpFileFactory::get_tmpFile();
        std::ofstream out_file(tmp_file.get_path(), std::ofstream::trunc);
        out_file<<header;
        std::vector<std::string> line;

        open_input_stream();
        while(not_eof()){
            line = getNextLine();
            if(condition(line)){
                line = update.apply(line);
            }
            out_file<<formatRow(line, delimiter, quotechar, endline);
        }
        close_input_stream();

        return csvFile(std::move(tmp_file), delimiter, endline, quotechar);
    }
}