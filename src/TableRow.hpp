#ifndef BIGCSV_TABLEROW_HPP
#define BIGCSV_TABLEROW_HPP

#include <map>
#include <string>
#include <vector>

namespace bigCSV{
    struct TableRow {

        std::vector<std::string> schema;            // Names of the columns
        std::vector<std::string> values;            // Values of the columns
        std::map<std::string, std::string> map;     // A map from names to values

        bool empty = true;      // If there are any values in the row

        // Add a name -> value pair to the row
        void add(const std::string& col_name, const std::string& value);

        // Convert into a vector<string> with a given schema
        std::vector<std::string> toLine(const std::vector<std::string> &input_schema) const;
    };
}

#endif 
