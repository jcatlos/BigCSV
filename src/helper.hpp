//
// Created by jakub on 15. 3. 2021.
//

#ifndef BIG_CSV_HELPER_HPP
#define BIG_CSV_HELPER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>

#include "TableRow.hpp"
#include "csvFile.hpp"

namespace bigCSV {

    // Given an input stream returns a token of a quoted string
        // The function does not check whether it is inside a quotes
        // Expects that the istream is 'on' the starting quotechar
    std::string getQuotedString(std::istream& in,
                                std::string::const_iterator &line_it,
                                std::string& line,
                                char quotechar,
                                char endline);

    // Given a string returns the same string without a leading and trailing whitespace
    std::string trimmedString(const std::string& str);

    // Given a vector of tokens, a delimiter, quotechar, and an endline characters returns these values formatted as a csv table row
    std::string formatRow(const std::vector<std::string>& cells, char delimiter, char quotechar, char endline);

    // Given 2 schemas returns a (set) union of these schemas
    std::vector<std::string> createJoinedSchema (const std::vector<std::string>& first, const std::vector<std::string>& second);

    // Returns the index of `str` in the `arr`
        // If not found, returns -1 - an always invalid index
    int index_of(std::string str, const std::vector<std::string>& arr);
}

#endif //BIG_CSV_HELPER_HPP
