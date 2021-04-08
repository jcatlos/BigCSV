//
// Created by jakub on 30. 3. 2021.
//

#include "file.hpp"
#include <iostream>

namespace bigCSV{ 

    File::File(File&& f) noexcept{
        path = std::move(f.path);
        temporary = f.temporary;
    }

    File::~File(){
        if(temporary && std::filesystem::exists(path)){
            std::error_code ec;
            std::filesystem::remove(path, ec);
        }
    }

}