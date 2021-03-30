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

#include "tmpFile.hpp"

namespace bigCSV {
    std::string getQuotedString(std::ifstream& input_stream,
                                std::string::const_iterator &line_it,
                                std::string& line,
                                char quotechar,
                                char endline);

    std::string trimmedString(const std::string& str);

    std::string formatRow(const std::vector<std::string>& cells, char delimiter, char quotechar, char endline);

    std::vector<std::string> split(const std::string& in, const char delim);

    bool setAttribute(const std::string& pair, std::map<std::string, std::string> attributes);

    std::string getNextWord(std::string& line, std::string::iterator& it);

    tmpFile getTmpFile();
}

#endif //BIG_CSV_HELPER_HPP
