//
// Created by jakub on 30. 3. 2021.
//

#include "RowComparator.hpp"

namespace bigCSV{

    bool RowComparator::compare(const std::map<std::string, std::string> &first_row,
                                const std::map<std::string, std::string> &second_row) {
        for(auto&& value: order){
            if(first_row.find(value) == first_row.end()) return false;
            if(second_row.find(value) == second_row.end()) return true;
            int result = first_row.at(value).compare(second_row.at(value));
            if(result < 0) return true;
            if(result > 0) return false;
        }
    }
}