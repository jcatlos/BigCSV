#include "RowComparator.hpp"
#include "helper.hpp"

namespace bigCSV{

    bool RowComparator::operator()(const TableRow &first_row, const TableRow &second_row) const{
        // In the order of importance of the columns, until a verdict is made
        for(auto&& value: order){
            // If a row does not contain a given column, the row containing the value is before
            if(first_row.map.find(value) == first_row.map.end()) return false;
            if(second_row.map.find(value) == second_row.map.end()) return true;

            // As all values are strings by default, we can apply string ordering
            int result = first_row.map.at(value).compare(second_row.map.at(value));
            // Check explcitly fo < / >, and if none is true, they are equal -> use another rov 
            if(result < 0) return true;
            if(result > 0) return false;
        }
        return true;
    }
}