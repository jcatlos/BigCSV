//
// Created by jakub on 2. 4. 2021.
//

#include <functional>
#include <vector>
#include <string>

#ifndef CATLOS_CONDITIONS_HPP
#define CATLOS_CONDITIONS_HPP

namespace bigCSV{
    class Conditions{
    private:
        std::vector<std::function<bool(const std::vector<std::string>&)>> conditions;
    public:
        bool Hold(const std::vector<std::string>& row) const;
        void AddEquals(int index, std::string str);
        void AddIntLt(int index, std::string str);
        void AddIntGt(int index, std::string str);
    };

    /*std::function<bool(const std::vector<std::string>&)> create_equal_check(int index, std::string str);
    std::function<bool(const std::vector<std::string>&)> create_int_lt(int index, std::string str);
    std::function<bool(const std::vector<std::string>&)> create_int_gt(int index, std::string str);
    bool tautology(const std::vector<std::string> &v);*/
}

#endif //CATLOS_CONDITIONS_HPP
