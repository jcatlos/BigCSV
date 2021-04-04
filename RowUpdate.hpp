//
// Created by jakub on 3. 4. 2021.
//

#include <functionaL>
#include <string>
#include <vector>
#include <map>

#ifndef CATLOS_ROWCHANGE_HPP
#define CATLOS_ROWCHANGE_HPP

namespace BigCSV{
    class RowUpdate {
    private:
        std::map<std::size_t, std::function<std::string(const std::string&)>> rowMap;

        static std::string _changeToImpl(const std::string& from, std::string to){
            return to;
        }

    public:
        void addChange(std::size_t index, const std::function<std::string(const std::string&)>& function);
        void addChange(std::size_t index, std::function<std::string(const std::string&)>&& function);

        std::vector<std::string> apply(const std::vector<std::string>& input_row);

        static std::function<std::string(const std::string&)> ChangeTo(std::string to){
            return std::bind(_changeToImpl, std::placeholders::_1,to);
        }
    };
}


#endif //CATLOS_ROWCHANGE_HPP
