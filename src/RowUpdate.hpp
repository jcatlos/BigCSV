#ifndef BIGCSV_ROWCHANGE_HPP
#define BIGCSV_ROWCHANGE_HPP

#include <functional>
#include <string>
#include <vector>
#include <map>

namespace BigCSV{

    class RowUpdate {
    private:
        // Stores transformations based on the index to be applied to
            // Only one update per colums
            // Could be modified to a vector of functions and the apply function modified
                // However now it is not needed
        std::map<std::size_t, std::function<std::string(const std::string&)>> rowMap;

        // This function is bound with a value 'to' when the modification is known
        static std::string _changeToImpl(const std::string& from, std::string to){
            return to;
        }

    public:
        // 2 overloads differentiating only in the type of the function passed
        void addChange(std::size_t index, const std::function<std::string(const std::string&)>& function);
        void addChange(std::size_t index, std::function<std::string(const std::string&)>&& function);

        // Returns a line containing a copy of the row with updated values
        std::vector<std::string> apply(const std::vector<std::string>& input_row) const;

        // Change the value to some other constant -  The only update implemented so far
        static std::function<std::string(const std::string&)> ChangeTo(std::string to){
            return std::bind(_changeToImpl, std::placeholders::_1,to);
        }
    };
}

#endif 
