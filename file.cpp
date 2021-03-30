//
// Created by jakub on 30. 3. 2021.
//

#include "file.hpp"
#include <iostream>

namespace bigCSV{

    File::File(std::filesystem::path p, bool temp)
            :path(p), temporary(temp)
    {
        //std::cout<<"Creating file: " << path << std::endl;
    }

    File::File(File&& f) noexcept{
        //std::cout<<"Moving file: " << f.path << std::endl;
        path = std::move(f.path);
        temporary = f.temporary;
    }

    File::~File(){
        if(temporary && std::filesystem::exists(path)){
            std::cout<<"Deleting temporary file: " << path << std::endl;
            std::error_code ec;
            std::filesystem::remove(path, ec);
            std::cout<<"Deleted with error code =  "<<" "<<ec.value()<<" "<<ec.message()<<std::endl;
        }
    }

}