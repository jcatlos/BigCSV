#ifndef BIGCSV_ROWCOMPARATOR_HPP
#define BIGCSV_ROWCOMPARATOR_HPP

#include <vector>
#include <map>
#include <string>

#include "TableRow.hpp"

namespace bigCSV{
    class RowComparator {
    private:
        // The columns ordered by in the order of importance
        std::vector<std::string> order;
    public:
        // Constructor takes a vector of column names in order of their importance
        explicit RowComparator(std::vector<std::string>&& o) : order(std::move(o)) {}
        
        // Compare 2 values based on the given order
        bool operator()(const TableRow& first_row, const TableRow& second_row) const;
    };
}


#endif
