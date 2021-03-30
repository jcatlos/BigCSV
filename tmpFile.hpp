//
// Created by jakub on 27. 3. 2021.
//

#include <filesystem>
#include <fstream>
#include "file.hpp"

#ifndef BIGCSV_TMPFILE_HPP
#define BIGCSV_TMPFILE_HPP

namespace bigCSV{
    class tmpFile : File{
    public:
        tmpFile() = delete;
        explicit tmpFile(std::filesystem::path p);
        tmpFile(const tmpFile& file) = delete;
        tmpFile(tmpFile&& file) noexcept;
        ~tmpFile();

        inline const std::filesystem::path get_path() const{
            return path;
        }

    };
}


#endif //BIGCSV_TMPFILE_HPP
