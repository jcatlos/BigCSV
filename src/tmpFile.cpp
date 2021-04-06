//
// Created by jakub on 27. 3. 2021.
//

#include <iostream>
#include "tmpFile.hpp"

namespace bigCSV{
    tmpFile::tmpFile(std::filesystem::path p) : File::File(p)
    {
        std::cout<<"Creating template file: " << path << std::endl;
    }

    tmpFile::tmpFile(tmpFile&& file) noexcept : File::File(std::move(file.path)){
        std::cout<<"Moving tmplate file: " << path << std::endl;
    }

    tmpFile::~tmpFile(){
        if(std::filesystem::exists(path)){
            std::cout<<"Deleting template file: " << path << std::endl;
            std::filesystem::remove(path);
        }
    }
}