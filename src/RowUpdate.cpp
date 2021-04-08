#include "RowUpdate.hpp"

namespace BigCSV{

    void RowUpdate::addChange(std::size_t index, const std::function<std::string(const std::string&)>& function){
        rowMap[index] = function;
    }

    void RowUpdate::addChange(std::size_t index, std::function<std::string(const std::string&)>&& function){
        rowMap[index] = function;
    }

    // Apply all functions on corresponding columns and return a copy of the resulting values
    std::vector<std::string> RowUpdate::apply(const std::vector<std::string>& input_row) const{
        std::vector<std::string> out = input_row;
        for(const auto& change: rowMap){
            out[change.first] = change.second(out[change.first]);
        }
        return out;
    }

}