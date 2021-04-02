#include <iostream>
#include <vector>
#include <functional>

#include "helper.hpp"
#include "csvTable.hpp"
#include "csvFile.hpp"
#include "shell.hpp"
#include "Conditions.hpp"

namespace bigCSV {

    void run(std::istream& in){

        // Variables for the whole program
        std::map<std::string, bigCSV::csvTable> tables;

        std::string line;

        tables["test"] = bigCSV::csvTable();
        tables["test"].addStream("input.csv", ',', '\n', '"');

        while(true){
            std::getline(in, line ,';');

            std::vector<std::string> command;
            std::string word = "";
            auto it = line.begin();
            while(it != line.end()){
                word = "";
                while(word.size() == 0){
                    word = getNextWord(line, it);
                }
                command.push_back(word);
            }

            word = command[0];

            // CREATE TABLE
            if(word == "CREATE") {
                if(command.size() < 3) std::cout<<"Invalid command form";
                std::string table_name = command[2];

                std::map<std::string, std::string> table_attributes;
                table_attributes["DELIMITER"] = ",";
                table_attributes["QUOTECHAR"] = "\"";
                table_attributes["ENDLINE"] = "\n";

                if(command[3] == "SET"){
                    for(int i=4; i<command.size(); i++){
                        if(!bigCSV::setAttribute(command[i], table_attributes)){
                            std::cout<<"Problem with attribute "<<i<<std::endl;
                            return;
                        }
                    }
                }
                else if(command.size() > 3){
                    std::cout<<"Error: Malformed CREATE TABLE query" <<std::endl;
                    return;
                }

                tables[table_name] = bigCSV::csvTable(
                        table_attributes["DELIMITER"][0],
                        table_attributes["QUOTECHAR"][0],
                        table_attributes["ENDLINE"][0]
                );
            }
            else if(word == "ALTER") {

            }
            else if(word == "INSERT") {

            }
            else if(word == "UPDATE") {

            }
            else if(word == "EXIT") {
                return;
            }
            else if(word == "SELECT") {

                std::cout<<"SELECT"<<std::endl;
                bool sort = false;
                bool from_file;
                RowComparator comparator((std::vector<std::string>()));
                std::vector<std::string> selected_columns;
                std::ofstream file_stream;

                int index = 1;                                          // Current position in the command;
                for(;command[index] != "FROM"; index++){
                    if(index == command.size()){
                        std::cout<<"Missing FROM clause"<< std::endl;
                        return;
                    }
                    std::cout<<command[index]<<std::endl;
                    selected_columns.push_back(command[index]);
                }
                index++;                                            // Skip the "FROM" token

                std::cout<<"Columns parsed"<<std::endl;

                // Find the specified table
                if(tables.find(command[index]) == tables.end()){
                    std::cout<<"ERROR Table "<<command[index]<<" Not found" << std::endl;
                    return;
                }
                bigCSV::csvTable& table = tables[command[index]];
                index++;

                std::cout<<"Table found"<<std::endl;

                std::function<bool(const std::vector<std::string>&)> condition = tautology;

                // Now the voluntary parts
                // WHERE - Now only 1 condition

                // Check for WHERE clause
                if(index < command.size() && command[index] == "WHERE") {
                    index++;
                    auto pair = split(command[index], '=');
                    if (pair.size() != 2) {
                        std::cout << "ERROR Malformed condition: " << command[index] << std::endl;
                        return;
                    }
                    int col_index = index_of(pair[0], selected_columns);
                    if (col_index < 0) {
                        std::cout << "ERROR " << pair[0] << " not in selected values" << std::endl;
                        return;
                    }
                    condition = create_equal_check(col_index, pair[1]);
                    index++;
                }
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
                    from_file = false;
                    file_stream.open(command[index]);
                    index++;
                }

                if(index < command.size()){
                    std::cout<<"Error: Malformed SELECT query" <<std::endl;
                    return;
                }

                std::cout<<"Launching Query"<<std::endl;

                std::ostream& out = (from_file ? file_stream : std::cout);
                if(sort){
                    table.sort(out, comparator, condition);
                }
                else table.printColumns(out, selected_columns, condition);


            }
            else {
                std::cout<<"ERROR Unknown command: "<<line<<std::endl;
                return;
            }
        }
    }
}