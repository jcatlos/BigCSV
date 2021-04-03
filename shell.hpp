#include <fstream>
#include <iostream>
#include <map>


#ifndef BIG_CSV_SHELL_HPP
#define BIG_CSV_SHELL_HPP

namespace bigCSV {
    class Shell{
    public:
        void run(std::istream& in);
    private:
        // Variables for the whole program
        std::map<std::string, bigCSV::csvTable> tables;
        std::string line;
        std::vector<std::string> command;

        void get_command(std::istream& in);

        std::map<std::string, char> get_attribute_map(int& index);

        void create();
        void select();
        void insert();
        void alter();
        void update();


    };
}

#endif //BIG_CSV_SHELL_HPP
