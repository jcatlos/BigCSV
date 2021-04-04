#include <iostream>
#include <vector>
#include <functional>

#include "helper.hpp"
#include "csvTable.hpp"
#include "csvFile.hpp"
#include "shell.hpp"
#include "Conditions.hpp"
#include "RowUpdate.hpp"

namespace bigCSV {

    csvTable* Shell::get_table(const std::string& name){
        auto table_it = tables.find(name);
        if(table_it == tables.end()){
            err_stream<<"ERROR: Table "<<name<<" Not found" << std::endl;
            return nullptr;
        }
        return &(table_it->second);
    }

     bool Shell::parse_where_clause(int& index, csvTable& table, Conditions& conds){
         if(index < command.size() && command[index] == "WHERE") {
             index++;
             while(index <= command.size()-3){
                 std::string first = command[index++];
                 std::string operand = command[index++];
                 if(operand != "=" && operand != "<" && operand != ">") return true;
                 std::string second = command[index++];

                 int col_index = index_of(first, table.schema);
                 if (col_index < 0) {
                     err_stream<<"ERROR: Column '"<<first<<"' is not in table '"<<command[1]<<"'"<<std::endl;
                     return false;
                 }
                 //std::cout<<"value = '"<<second<<"' at "<<col_index<<std::endl;
                 switch(operand[0]){
                     case '=':
                         conds.AddEquals(col_index, second);
                         break;
                     case '<':
                         conds.AddIntLt(col_index, second);
                         break;
                     case '>':
                         conds.AddIntGt(col_index, second);
                         break;
                     default:
                         err_stream<<"ERROR: Unknown Condition '"<<operand<<"'"<<std::endl;
                         break;
                 }
             }
         }
         return true;
    }

    bool Shell::set_map_attribute(const std::string& pair, std::map<std::string, std::string>& attributes){
        auto result = split(pair, '=');
        // Invalid form of attribute setting
        if(result.size() != 2) return false;
        auto key_it = attributes.find(result[0]);
        // Invalid attribute name
        if(key_it == attributes.end()) return false;
        key_it->second = result[1];
        return true;
    }

    std::map<std::string, std::string> Shell::get_attribute_map(int& index){
        std::map<std::string, std::string> atts;
        atts["DELIMITER"] = ",";
        atts["QUOTECHAR"] = "\"";
        atts["ENDLINE"] = "\n";
        atts["OUT_DELIMITER"] = ",";
        atts["OUT_QUOTECHAR"] = "\"";
        atts["OUT_ENDLINE"] = "\n";
        atts["MAX_FILESIZE"] = "13500";

        if(modify_attribute_map(index, atts)){
            return atts;
        }
        else{
            return std::map<std::string, std::string>();
        }
    }

    bool Shell::modify_attribute_map(int& index, std::map<std::string, std::string>& atts){
        // If there are attributes to be parsed, parse them
        if(index < command.size() && command[index] == "SET"){
            int start_index = index;
            index++;
            for(;index<command.size(); index++){
                if(!set_map_attribute(command[index], atts)){
                    err_stream<<"Problem with attribute "<<index - start_index<<std::endl;
                    return false;
                }
            }
        }
        return true;
    }

    // Returns the next word in the line
        // Treats quoted strings as single words
        // in quoted strings, escaping via '\' is supported
    std::string Shell::getNextWord(std::string& line, std::string::iterator& it){
        std::string out = "";
        while(it != line.end() && isspace(*it)) it++;        // Take all the whitespace before the next word
        bool quoted = *it == '"';

        // If the word is in quotes, look until the next quotes
        if(quoted){
            it++;
            while(it != line.end() && *it != '"'){
                // Escaping
                if(*it == '\\'){
                    it++;
                    if(it == line.end()) return "";
                }

                out += *it;
                it++;
            }
            it++;
        }

        // Otherwise, look for the next space
        else{
            while(it != line.end() && !isspace(*it)){
                out += *it;
                it++;
            }
        }
        return out;
    }

    void Shell::get_command(std::istream &in) {
        command.clear();
        std::getline(in, line ,';');
        std::string word = "";
        auto it = line.begin();
        while(it != line.end()){
            word = "";
            while(word.empty()){
                word = getNextWord(line, it);
            }
            command.push_back(word);
        }
    }

    void Shell::create(){
        if(command.size() < 3 && command[1]!="TABLE"){
            err_stream<<"Invalid command form";
            return;
        }
        std::string table_name = command[2];

        int index = 3;
        std::map<std::string, std::string> table_attributes = get_attribute_map(index);

        if(table_attributes.empty()){
            err_stream<<"Problems occured during attribute parsing, Aborting CREATE"<<std::endl;
            return;
        }

        tables[table_name] = bigCSV::csvTable(
                table_attributes["DELIMITER"][0],
                table_attributes["QUOTECHAR"][0],
                table_attributes["ENDLINE"][0]
        );
    }

    void Shell::update() {
        BigCSV::RowUpdate update;
        Conditions conds;

        // Check the minimal command length and form
        if(command.size() < 4 || command[2] != "SET"){
            err_stream<<"ERROR: Invalid UPDATE command form";
            return;
        }

        // Find the specified table
        bigCSV::csvTable* table = get_table(command[1]);
        if(table == nullptr) return;

        // Extract all updates
        int index = 3;
        while(index < command.size() && command[index] != "WHERE"){
            auto pair = split(command[index], '=');
            if(pair.size() != 2){
                err_stream<<"ERROR: Invalid update '"<<command[index]<<"'"<<std::endl;
                return;
            }
            int col_index = index_of(pair[0], table->schema);
            if(col_index == -1) {
                err_stream<<"ERROR: Column '"<<pair[0]<<"' is not in table '"<<command[1]<<"'"<<std::endl;
                return;
            }
            update.addChange(col_index, BigCSV::RowUpdate::ChangeTo(pair[1]));
            index++;
        }

        // Check for WHERE clause
        if(!parse_where_clause(index, *table, conds)) return;

        table->updateTable(conds, update);

    }

    void Shell::select() {
        //std::cout<<"SELECT"<<std::endl;
        bool sort = false;                                              // If the output has to be sorted
        bool to_file = false;                                           // If the output is directed into a file (otherwise goes to std::cout)
        std::ofstream file_stream;
        std::vector<std::string> selected_columns;                      // Columns to be printed
        RowComparator comparator((std::vector<std::string>()));      // Comparator to be used for sorting (default is an implicit one)
        Conditions conds;                                               // The condition for filtering of the results (default is "none")

        // Add all provided column names into selected_columns
        int index = 1;                                          // Current position in the command;
        for(;command[index] != "FROM"; index++){
            if(index == command.size()){
                err_stream<<"Missing FROM clause"<< std::endl;
                return;
            }
            selected_columns.push_back(command[index]);
        }
        index++;                                            // Skip the "FROM" token

        // Find the specified table
        bigCSV::csvTable* table = get_table(command[index]);
        if(table == nullptr) return;
        index++;

        // Now the voluntary parts

        // Check for WHERE clause
        if(!parse_where_clause(index, *table, conds)) return;

        // Check for ORDER BY clause
        if(index < command.size() && command[index] == "ORDER"){
            index++;
            if(command[index] == "BY") index++;     // Skip "BY"
            sort = true;
            std::vector<std::string> order_by;
            while(index<command.size() && command[index] != "INTO"){
                order_by.push_back(command[index]);
                index++;
            }
            comparator = RowComparator(std::move(order_by));
        }
        // Check for "INTO" clause
        if(index < command.size() && command[index] == "INTO"){
            index++;
            to_file = true;
            file_stream.open(command[index]);
            index++;
        }

        if(index < command.size()){
            err_stream<<"Error: Malformed SELECT query" <<std::endl;
            return;
        }

        std::ostream& out = (to_file ? file_stream : std::cout);

        if(sort) table->sort(out, comparator, conds, selected_columns);
        else table->printColumns(out, selected_columns, conds);
    }

    void Shell::insert(){
        // Check the form of the statement
        if(command.size() < 5 || command[1] != "INTO" || command[3] != "PATH" ){
            err_stream<<"Invalid command form";
            return;
        }

        // Find the specified table
        bigCSV::csvTable* table = get_table(command[2]);
        if(table == nullptr) return;

        auto path = command[4];
        int index = 5;
        std::map<std::string, std::string> file_attributes = get_attribute_map(index);

        if(file_attributes.empty()){
            err_stream<<"Problems occured during attribute parsing, Aborting INSERT INTO"<<std::endl;
            return;
        }

        table->addFile(path, file_attributes["DELIMITER"][0], file_attributes["ENDLINE"][0],
                                   file_attributes["QUOTECHAR"][0]);

    }

    void Shell::alter(){
        if(command.size() < 3 || command[1] != "TABLE"){
            err_stream<<"Invalid command form";
            return;
        }

        // Find the specified table
        csvTable* table = get_table(command[2]);
        if(table == nullptr) return;

        int index = 3;
        std::map<std::string, std::string>table_attributes;
        table_attributes["DELIMITER"] = table->in_delimiter;
        table_attributes["QUOTECHAR"] = table->in_quotechar;
        table_attributes["ENDLINE"] = table->in_endline;
        table_attributes["OUT_DELIMITER"] = table->out_delimiter;
        table_attributes["OUT_QUOTECHAR"] = table->out_quotechar;
        table_attributes["OUT_ENDLINE"] = table->out_endline;
        table_attributes["MAX_FILESIZE"] = std::to_string(table->max_filesize);

        if(!modify_attribute_map(index, table_attributes)){
            err_stream<<"Problems occured during attribute parsing, Aborting ALTER"<<std::endl;
            return;
        }

        try{
            table->max_filesize = std::stoi(table_attributes["MAX_FILESIZE"]);
            if(table->max_filesize < 1) throw std::invalid_argument("File size is less than 1");
        }
        catch (std::invalid_argument){
            err_stream << "Error: MAX_FILESIZE is not a non-negative integer"<<std::endl;
            return;
        }

        table->in_delimiter = table_attributes["DELIMITER"][0];
        table->in_quotechar = table_attributes["QUOTECHAR"][0];
        table->in_endline = table_attributes["ENDLINE"][0];
        table->out_delimiter = table_attributes["OUT_DELIMITER"][0];
        table->out_quotechar = table_attributes["OUT_QUOTECHAR"][0];
        table->out_endline = table_attributes["OUT_ENDLINE"][0];

        return;
    }

    void Shell::run(){
        while(true){
            get_command(in_stream);
            std::string word = command[0];

            // CREATE TABLE
            if(word == "CREATE") {
                create();
            }
            else if(word == "ALTER") {
                alter();
            }
            else if(word == "INSERT") {
                insert();
            }
            else if(word == "UPDATE") {
                update();
            }
            else if(word == "EXIT") {
                err_stream<<"SESSION ENDED SUCCESSFULLY"<<std::endl;
                return;
            }
            else if(word == "SELECT") {
                select();
            }
            else {
                err_stream<<"ERROR Unknown command: "<<line<<std::endl;
            }
        }
    }
}