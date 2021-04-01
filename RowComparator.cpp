//
// Created by jakub on 30. 3. 2021.
//

#include "RowComparator.hpp"
#include "helper.hpp"

namespace bigCSV{

    bool RowComparator::operator()(const TableRow &first_row, const TableRow &second_row) const{
        for(auto&& value: order){
            if(first_row.map.find(value) == first_row.map.end()) return false;
            if(second_row.map.find(value) == second_row.map.end()) return true;
            int result = first_row.map.at(value).compare(second_row.map.at(value));
            if(result < 0) return true;
            if(result > 0) return false;
        }
    }
}