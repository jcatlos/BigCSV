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

     bool Shell::parse_where_clause(int& index, csvTable& table, std::function<bool(const std::vector<std::string>&)>& condition){
        if(index < command.size() && command[index] == "WHERE") {
            index++;
            auto pair = split(command[index], '=');
            if (pair.size() != 2) {
                std::cout << "ERROR Malformed condition: " << command[index] << std::endl;
                return false;
            }
            int col_index = index_of(pair[0], table.schema);
            if (col_index < 0) {
                std::cout<<"ERROR: Column '"<<pair[0]<<"' is not in table '"<<command[1]<<"'"<<std::endl;
                return false;
            }
            std::cout<<"value = '"<<pair[1]<<"' at "<<col_index<<std::endl;
            condition = create_equal_check(col_index, pair[1]);
            index++;
        }

        return true;
    }

    bool Shell::set_map_attribute(const std::string& pair, std::map<std::string, char>& attributes){
        auto result = split(pair, '=');
        // Invalid form of attribute setting
        if(result.size() != 2) return false;
        auto key_it = attributes.find(result[0]);
        // Invalid attribute name
        if(key_it == attributes.end()) return false;
        key_it->second = result[1][0];
        return true;
    }

    std::map<std::string, char> Shell::get_attribute_map(int& index){
        std::map<std::string, char> atts;
        atts["DELIMITER"] = ',';
        atts["QUOTECHAR"] = '"';
        atts["ENDLINE"] = '\n';
        atts["OUT_DELIMITER"] = ',';
        atts["OUT_QUOTECHAR"] = '"';
        atts["OUT_ENDLINE"] = '\n';

        if(modify_attribute_map(index, atts)){
            return atts;
        }
        else{
            return std::map<std::string, char>();
        }
    }



    bool Shell::modify_attribute_map(int& index, std::map<std::string, char>& atts){
        // If there are attributes to be parsed, parse them
        if(index < command.size() && command[index] == "SET"){
            int start_index = index;
            index++;
            for(;index<command.size(); index++){
                if(!set_map_attribute(command[index], atts)){
                    std::cout<<"Problem with attribute "<<index - start_index<<std::endl;
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
            std::cout<<"Invalid command form";
            return;
        }
        std::string table_name = command[2];

        int index = 3;
        std::map<std::string, char> table_attributes = get_attribute_map(index);

        if(table_attributes.empty()){
            std::cout<<"Problems occured during attribute parsing, Aborting CREATE"<<std::endl;
            return;
        }

        tables[table_name] = bigCSV::csvTable(
                table_attributes["DELIMITER"],
                table_attributes["QUOTECHAR"],
                table_attributes["ENDLINE"]
        );
    }

    void Shell::update() {
        BigCSV::RowUpdate update;
        std::function<bool(const std::vector<std::string>&)> condition = tautology;

        // Check the minimal command length and form
        if(command.size() < 4 || command[2] != "SET"){
            std::cout<<"ERROR: Invalid UPDATE command form";
            return;
        }

        // Find the specified table
        auto table_it = tables.find(command[1]);
        if(table_it == tables.end()){
            std::cout<<"ERROR: Table "<<command[1]<<" Not found" << std::endl;
            return;
        }
        bigCSV::csvTable& table = tables[command[1]];

        // Extract all updates
        int index = 3;
        while(index < command.size() && command[index] != "WHERE"){
            auto pair = split(command[index], '=');
            if(pair.size() != 2){
                std::cout<<"ERROR: Invalid update '"<<command[index]<<"'"<<std::endl;
                return;
            }
            int col_index = index_of(pair[0], table.schema);
            if(col_index == -1) {
                std::cout<<"ERROR: Column '"<<pair[0]<<"' is not in table '"<<command[1]<<"'"<<std::endl;
                return;
            }
            update.addChange(col_index, BigCSV::RowUpdate::ChangeTo(pair[1]));
            index++;
        }

        // Check for WHERE clause
        if(!parse_where_clause(index, table, condition)) return;

        table.updateTable(condition, update);

    }

    void Shell::select() {
        //std::cout<<"SELECT"<<std::endl;
        bool sort = false;                                                              // If the output has to be sorted
        bool to_file = false;                                                           // If the output is directed into a file (otherwise goes to std::cout)
        std::ofstream file_stream;
        std::vector<std::string> selected_columns;                                      // Columns to be printed
        RowComparator comparator((std::vector<std::string>()));                      // Comparator to be used for sorting (default is an implicit one)
        std::function<bool(const std::vector<std::string>&)> condition = tautology;     // The condition for filtering of the results (default is "none")

        // Add all provided column names into selected_columns
        int index = 1;                                          // Current position in the command;
        for(;command[index] != "FROM"; index++){
            if(index == command.size()){
                std::cout<<"Missing FROM clause"<< std::endl;
                return;
            }
            //std::cout<<command[index]<<std::endl;
            selected_columns.push_back(command[index]);
        }
        index++;                                            // Skip the "FROM" token

        // Find the specified table
        auto table_it = tables.find(command[index]);
        if(table_it == tables.end()){
            std::cout<<"ERROR Table "<<command[index]<<" Not found" << std::endl;
            return;
        }
        bigCSV::csvTable& table = tables[command[index]];
        index++;

        //std::cout<<"Table found"<<std::endl;

        // Now the voluntary parts

        // Check for WHERE clause
        if(!parse_where_clause(index, table, condition)) return;

        // Check for ORDER BY clause
        if(index < command.size() && command[index] == "ORDER"){
            index++;
            if(command[index] == "BY") index++;     // Skip "BY"
            sort = true;
            std::vector<std::string> order_by;
            while(index<command.size() && command[index] != "INTO"){
                std::cout<<command[index]<<std::endl;
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
            std::cout<<"Error: Malformed SELECT query" <<std::endl;
            return;
        }

        std::cout<<"Launching Query"<<std::endl;

        std::ostream& out = (to_file ? file_stream : std::cout);

        if(sort) table.sort(out, comparator, condition, selected_columns);
        else table.printColumns(out, selected_columns, condition);
    }

    void Shell::insert(){
        // Check the form of the statement

        std::cout<<"INSERT STARTED"<<std::endl;

        if(command.size() < 5 || command[1] != "INTO" || command[3] != "PATH" ){
            std::cout<<"Invalid command form";
            return;
        }

        auto table_name = command[2];
        auto path = command[4];

        // Find the specified table
        auto table_it = tables.find(table_name);
        if(table_it == tables.end()){
            std::cout<<"Error: Table \""<<table_name<<"\" Not found"<<std::endl;
            return;
        }

        int index = 5;
        std::map<std::string, char> file_attributes = get_attribute_map(index);
        //std::cout<<"att = "<<table_attributes["QUOTECHAR"]<<std::endl;

        if(file_attributes.empty()){
            std::cout<<"Problems occured during attribute parsing, Aborting INSERT INTO"<<std::endl;
            return;
        }

        tables[table_name].addFile(path, file_attributes["DELIMITER"], file_attributes["ENDLINE"],
                                   file_attributes["QUOTECHAR"]);

    }

    void Shell::alter(){
        std::cout<<"ALTER"<<std::endl;

        if(command.size() < 3 || command[1] != "TABLE"){
            std::cout<<"Invalid command form";
            return;
        }

        // Find the specified table
        std::string table_name = command[2];
        auto table_it = tables.find(table_name);
        if(table_it == tables.end()){
            std::cout<<"Error: Table \""<<table_name<<"\" Not found"<<std::endl;
            return;
        }
        auto& table = tables[table_name];

        int index = 3;
        std::map<std::string, char>table_attributes;
        table_attributes["DELIMITER"] = table.in_delimiter;
        table_attributes["QUOTECHAR"] = table.in_quotechar;
        table_attributes["ENDLINE"] = table.in_endline;
        table_attributes["OUT_DELIMITER"] = table.out_delimiter;
        table_attributes["OUT_QUOTECHAR"] = table.out_quotechar;
        table_attributes["OUT_ENDLINE"] = table.out_endline;

        if(!modify_attribute_map(index, table_attributes)){
            std::cout<<"Problems occured during attribute parsing, Aborting ALTER"<<std::endl;
            return;
        }

        table.in_delimiter = table_attributes["DELIMITER"];
        table.in_quotechar = table_attributes["QUOTECHAR"];
        table.in_endline = table_attributes["ENDLINE"];
        table.out_delimiter = table_attributes["OUT_DELIMITER"];
        table.out_quotechar = table_attributes["OUT_QUOTECHAR"];
        table.out_endline = table_attributes["OUT_ENDLINE"];
    }

    void Shell::run(std::istream& in){

        while(true){
            get_command(in);
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
                return;
            }
            else if(word == "SELECT") {
                select();
            }
            else {
                std::cout<<"ERROR Unknown command: "<<line<<std::endl;
            }
        }
    }
}