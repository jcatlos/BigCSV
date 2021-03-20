#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

#include "csvFile.hpp"

std::string getNextWord(std::string& line, std::string::iterator& it){
    std::string out = "";
    while(it != line.end() && isspace(*it)) it++;        // Take all the whitespace before the next word
    while(it != line.end() && !isspace(*it)){
        out += *it;
        it++;
    }
    return out;
}

// CREATE TABLE table_name [SET att=val, … ]
void parseCreate(std::vector<std::string>& command){
    if(command.size() < 3){
        std::cout<<"Invalid command form";
    }

    std::string table_name = command[2];

    std::cout<<"CREATE TABLE "<<table_name;

    if(command[3] == "SET"){
        for(int i=4; i<command.size(); i++){
            // Spracuj dvojicu att=val;
            std::cout<<" att=val";
        }
    }
    else if(command.size() > 3){
        std::cout<<"Error: Malformed CREATE TABLE query" <<std::endl;
        return;
    }

    // Vytvor tabulku

    std::cout<<std::endl;
}

void loadCommand(std::istream& in){
    std::string line;
    std::getline(in, line ,';');

    std::vector<std::string> command;
    std::string word = "";
    auto it = line.begin();
    while(it != line.end()){
        std::cout<<"here"<<std::endl;
        word = "";
        while(word.size() == 0){
            word = getNextWord(line, it);
        }
        command.push_back(word);
    }

    word = command[0];

    if(word == "CREATE") {
        parseCreate(command);
    }
    else if(word == "ALTER") {

    }
    else if(word == "INSERT") {

    }
    else if(word == "UPDATE") {

    }
    else if(word == "SELECT") {

    }
    else {
        std::cout<<"ERROR Unknown command: "<<line<<std::endl;
    }

}

int main() {
    //bigCSV::csvFile file("input.csv", ',', '\n', '"');
    //file.printFile();
    std::cout<<"Im alive"<<std::endl;

    loadCommand(std::cin);

    return 0;
}
