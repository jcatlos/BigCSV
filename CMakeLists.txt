cmake_minimum_required(VERSION 3.15)
project(catlos)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-lstdc++fs" )

add_executable(catlos
        src/csvFile.cpp
        src/csvFile.hpp
        src/csvTable.cpp
        src/csvTable.hpp
        src/helper.cpp
        src/helper.hpp
        src/main.cpp
        src/shell.cpp
        src/shell.hpp
        src/tmpFileFactory.cpp
        src/tmpFileFactory.hpp
        src/file.cpp
        src/file.hpp
        src/RowComparator.cpp
        src/RowComparator.hpp
        src/TableRow.cpp
        src/TableRow.hpp 
        src/Conditions.cpp 
        src/Conditions.hpp 
        src/RowUpdate.cpp 
        src/RowUpdate.hpp)

