#ifndef BIGCSV_TMPFILEFACTORY_HPP
#define BIGCSV_TMPFILEFACTORY_HPP

#include <filesystem>
#include "file.hpp"

namespace bigCSV{

    // A Singleton used to create unique temporary files
        // By being a Singleton, it is the only authority to create files 
            // -> Guaranteed uniqueness across the program

    class tmpFileFactory {
    public:
        // Creates an instance of File with an unique name and the temporary bit set to true
            // Called as tmpFileFactory::get_tmpFile()
        static File get_tmpFile(){
            return get()._get_file_impl();
        }

        // Singleton declarations
        tmpFileFactory(const tmpFileFactory&) = delete;
        static tmpFileFactory& get() {
            return _instance;
        }
    private:
        // File names are created as "[_tmp_dir]/tmp_file_[_file_count].csv"
        std::size_t _file_count = 0;
        std::filesystem::path _tmp_dir = std::filesystem::temp_directory_path();
        File _get_file_impl();

        // Singleton declaration
        tmpFileFactory()= default;
        static tmpFileFactory _instance;
    };
}

#endif
