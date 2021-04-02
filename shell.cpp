#include <iostream>
#include <vector>

#include "helper.hpp"
#include "csvTable.hpp"
#include "csvFile.hpp"
#include "shell.hpp"

namespace bigCSV {
    void run(std::istream& in){

        // Variables for the whole program
        std::map<std::string, bigCSV::csvTable> tables;


        std::string line;
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
        else if(word == "SELECT") {
            std::vector<std::string> selected_columns;
            int index;                                          // Current position in the command;
            for(; command[index] != "FROM"; index++){
                selected_columns.push_back(command[index]);
            }
            index++;                                            // Skip the "FROM" token
            // Find the specified table
            csvTable table;
            if(tables.find(command[index]) == tables.end()){
                std::cout<<"ERROR Table "<<command[index]<<" Not found" << std::endl;
                return;
            }
            else {
                //table = &tables[command[index]];
            }

            // WHERE

        }
        else {
            std::cout<<"ERROR Unknown command: "<<line<<std::endl;
        }

    }
}