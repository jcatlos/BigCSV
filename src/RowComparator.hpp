//
// Created by jakub on 30. 3. 2021.
//

#ifndef CATLOS_ROWCOMPARATOR_HPP
#define CATLOS_ROWCOMPARATOR_HPP

#include <vector>
#include <map>
#include <string>
#include "TableRow.hpp"

namespace bigCSV{
    class RowComparator {
    private:
        std::vector<std::string> order;
    public:
        explicit RowComparator(std::vector<std::string>&& o) : order(std::move(o)) {}
        bool operator()(const TableRow& first_row, const TableRow& second_row) const;
    };
}


#endif //CATLOS_ROWCOMPARATOR_HPP
