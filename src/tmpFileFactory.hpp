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
        tmpFileFactory(const tmpFileFactory&) = delete;

        static tmpFileFactory& get() {
            return _instance;
        }

        static File get_tmpFile(){
            return get()._get_file_impl();
        }
    private:
        File _get_file_impl();
        tmpFileFactory()= default;

        int _file_count = 0;
        std::filesystem::path _tmp_dir = std::filesystem::temp_directory_path();

        static tmpFileFactory _instance;
    };
}

#endif
