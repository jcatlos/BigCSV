//
// Created by jakub on 30. 3. 2021.
//

#ifndef CATLOS_ROWCOMPARATOR_HPP
#define CATLOS_ROWCOMPARATOR_HPP

#include <vector>
#include <map>
#include <string>

namespace bigCSV{
    class RowComparator {
    private:
        std::vector<std::string> order;
    public:
        RowComparator(std::vector<std::string> o) : order(std::move(o)) {}
        bool compare(const std::map<std::string, std::string>& first_row,
                     const std::map<std::string, std::string>& second_row);
    };
}


#endif //CATLOS_ROWCOMPARATOR_HPP
