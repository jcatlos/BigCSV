//
// Created by jakub on 31. 3. 2021.
//

#include "TableRow.hpp"

namespace bigCSV{

    void TableRow::add(const std::string &col_name, const std::string &value) {
        if(map.find(col_name) == map.end()){
            map[col_name] = value;
            schema.push_back(col_name);
            values.push_back(value);
        }
        else{
            map[col_name] = value;
            for(int i=0; i<schema.size(); i++){
                if(schema[i] == col_name){
                    values[i] = value;
                    break;
                }
            }
        }
    }

    std::vector<std::string> TableRow::toLine(const std::vector<std::string>& input_schema){
        std::vector<std::string> out;
        for(const auto& col_name : input_schema){
            if(map.find(col_name) == map.end()){
                out.push_back("");
            }
            else{
                out.push_back(map[col_name]);
            }
        }
        return out;
    }
}