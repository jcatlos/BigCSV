//
// Created by jakub on 2. 4. 2021.
//

#include "Conditions.hpp"

namespace bigCSV{


    bool int_lt(const std::vector<std::string>& row, int index, std::string num){
        if(index >= row.size()) return false;
        try {
            return std::stoi(row[index]) < std::stoi(num);
        }
        catch (std::exception& e) {
            return false;
        }
    }

    bool int_gt(const std::vector<std::string>& row, int index, std::string num){
        if(index >= row.size()) return false;
        try {
            return std::stoi(row[index]) > std::stoi(num);
        }
        catch (std::exception& e) {
            return false;
        }
    }

    bool equals(const std::vector<std::string>& row, int index, std::string str){
        if(index >= row.size()) return false;
        return row[index]==str;
    }

    /*std::function<bool(const std::vector<std::string> &)> create_equal_check(int index, std::string str) {
        return std::bind(equals, std::placeholders::_1, index, str);
    }

    std::function<bool(const std::vector<std::string> &)> create_int_lt(int index, std::string str) {
        return std::bind(int_lt, std::placeholders::_1, index, str);
    }

    std::function<bool(const std::vector<std::string> &)> create_int_gt(int index, std::string str) {
        return std::bind(int_gt, std::placeholders::_1, index, str);
    }*/

    /*bool tautology(const std::vector<std::string> &v) {
        return true;
    }*/

    bool Conditions::Hold(const std::vector<std::string> &row) const {
        for(const auto& cond : conditions){
            if(!cond(row)) return false;
        }
        return true;
    }

    void Conditions::AddEquals(int index, std::string str) {
        conditions.push_back(std::bind(equals, std::placeholders::_1, index, str));
    }

    void Conditions::AddIntLt(int index, std::string str) {
        conditions.push_back(std::bind(int_lt, std::placeholders::_1, index, str));
    }

    void Conditions::AddIntGt(int index, std::string str) {
        conditions.push_back(std::bind(int_gt, std::placeholders::_1, index, str));
    }
}
