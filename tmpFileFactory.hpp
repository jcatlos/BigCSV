//
// Created by jakub on 30. 3. 2021.
//

#ifndef CATLOS_TMPFILEFACTORY_HPP

#define CATLOS_TMPFILEFACTORY_HPP
#include <filesystem>
#include "file.hpp"

namespace bigCSV{
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


#endif //CATLOS_TMPFILEFACTORY_HPP
