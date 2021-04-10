#ifndef BIGCSV_CONDITIONS_HPP
#define BIGCSV_CONDITIONS_HPP

#include <functional>
#include <vector>
#include <string>

namespace bigCSV{
    class Conditions{
    private:
        std::vector<std::function<bool(const std::vector<std::string>&)>> conditions;
    public:
        bool Hold(const std::vector<std::string>& row) const;
        void AddEquals(int index, std::string str);
        void AddIntLt(int index, std::string str);
        void AddIntGt(int index, std::string str);
        void AddOther(std::function<bool(const std::vector<std::string>&)>&& condition);
    };
}

#endif 
