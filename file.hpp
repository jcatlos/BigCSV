//
// Created by jakub on 30. 3. 2021.
//

#ifndef CATLOS_FILE_HPP
#define CATLOS_FILE_HPP

#include <filesystem>

namespace bigCSV{
    class File {
    private:
        std::filesystem::path path;
        bool temporary;
    public:
        File() = delete;
        File(std::filesystem::path p, bool temp);
        explicit File(std::filesystem::path p) :path(p), temporary(false) {}
        File(const File& file) = delete;
        File(File&& file) noexcept;
        ~File();

        inline const std::filesystem::path get_path() const{
            return path;
        }

    };
}


#endif //CATLOS_FILE_HPP
