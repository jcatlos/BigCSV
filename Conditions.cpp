//
// Created by jakub on 2. 4. 2021.
//

#include "Conditions.hpp"

namespace bigCSV{

    bool equals(const std::vector<std::string>& col, int index, std::string str){
        return col[index]==str;
    }

    std::function<bool(const std::vector<std::string> &)> create_equal_check(int index, std::string str) {
        return std::bind(equals, std::placeholders::_1, index, str);
    }

    bool tautology(const std::vector<std::string> &v) {
        return true;
    }
}
