#ifndef BIGCSV_FILE_HPP
#define BIGCSV_FILE_HPP

#include <filesystem>

namespace bigCSV{
    class File {
    private:
        std::filesystem::path path;
        bool temporary;         // If a file is temporary it will be deleted on destruction

    public:
        // A file may not be created without an assigned path
        File() = delete;

        // Constructors 
        File(std::filesystem::path p, bool temp) 
            : path(p), temporary(temp){}
        explicit File(std::filesystem::path p) 
            : path(p), temporary(false) {}

        // File handles may not be copied, however they may be moved
        File(const File& file) = delete;
        File(File&& file) noexcept;

        // Deletes a file if it is temporary
        ~File();

        // A getter for the file's path
        inline const std::filesystem::path& get_path() const{
            return path;
        }

    };
}


#endif 
