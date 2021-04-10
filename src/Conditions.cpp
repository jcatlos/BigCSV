#include "Conditions.hpp"

namespace bigCSV{

    // -------- INTERNAL FUNCTORS TO BE BOUND BY APPROPRIATE FUNCTIONS ---------//
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

   
    // Evaluates whether the contitions hold on a given row 
    bool Conditions::Hold(const std::vector<std::string> &row) const {
        for(const auto& cond : conditions){
            if(!cond(row)) return false;
        }
        return true;
    }

    // Adds a bound string equality to constant condition
    void Conditions::AddEquals(int index, std::string str) {
        conditions.push_back(std::bind(equals, std::placeholders::_1, index, str));
    }

    // These 2 add integer comparisons to a constant condition
    void Conditions::AddIntLt(int index, std::string str) {
        conditions.push_back(std::bind(int_lt, std::placeholders::_1, index, str));
    }

    void Conditions::AddIntGt(int index, std::string str) {
        conditions.push_back(std::bind(int_gt, std::placeholders::_1, index, str));
    }

    // Allows for inserting a different function
    void Conditions::AddOther(std::function<bool(const std::vector<std::string>&)>&& condition){
        conditions.push_back(condition);
    }
}
