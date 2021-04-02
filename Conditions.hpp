//
// Created by jakub on 2. 4. 2021.
//

#include <functional>
#include <vector>
#include <string>

#ifndef CATLOS_CONDITIONS_HPP
#define CATLOS_CONDITIONS_HPP

namespace bigCSV{
    std::function<bool(const std::vector<std::string>&)> create_equal_check(int index, std::string str);

    bool tautology(const std::vector<std::string> &v);
}

#endif //CATLOS_CONDITIONS_HPP
