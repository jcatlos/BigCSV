//
// Created by jakub on 15. 3. 2021.
//

#ifndef BIG_CSV_HELPER_HPP
#define BIG_CSV_HELPER_HPP

#include <string>
#include <fstream>
#include <vector>

namespace bigCSV {
    std::string getQuotedString(std::ifstream& input_stream,
                                std::string::const_iterator &line_it,
                                std::string& line,
                                char quotechar,
                                char endline);

    std::string trimmedString(const std::string& str);

    std::string formatRow(const std::vector<std::string>& cells, char delimiter, char quotechar, char endline);
}

#endif //BIG_CSV_HELPER_HPP
