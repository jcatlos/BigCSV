//
// Created by jakub on 30. 3. 2021.
//

#include <string>

#include "tmpFileFactory.hpp"

namespace bigCSV{
    File tmpFileFactory::_get_file_impl() {
        std::string file_name = "tmp_file_" + std::to_string(_file_count++) + ".csv";
        return File(_tmp_dir / file_name, true);
    }

    tmpFileFactory tmpFileFactory::_instance;
}