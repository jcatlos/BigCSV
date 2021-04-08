
#include "TableRow.hpp"

namespace bigCSV{

    // Adds a name -> value pair to the row
    void TableRow::add(const std::string &col_name, const std::string &value) {
        // If the name is not in the row, add it
        if(map.find(col_name) == map.end()){
            map[col_name] = value;
            schema.push_back(col_name);
            values.push_back(value);
        }
        // Otherwise modify the value
        else{
            map[col_name] = value;
            for(int i=0; i<schema.size(); i++){
                if(schema[i] == col_name){
                    values[i] = value;
                    return;
                }
            }
        }
    }

    // Converts the row into a vector<string> of values in a given schema
    std::vector<std::string> TableRow::toLine(const std::vector<std::string>& input_schema) const {
        std::vector<std::string> out;
        for(const auto& col_name : input_schema){
            // If a value with the given col_name is *not* in the row, add an empty value
            if(map.find(col_name) == map.end()){
                out.push_back("");
            }
            // Otherwise just add the value
            else{
                out.push_back(map.at(col_name));
            }
        }
        return out;
    }
}