#include <fstream>
#include <iostream>
#include <map>


#ifndef BIG_CSV_SHELL_HPP
#define BIG_CSV_SHELL_HPP

namespace bigCSV {
    class Shell{
    public:
        Shell(std::istream& in, std::ostream& err) : err_stream(err), in_stream(in) {}
        void run();
    private:
        std::ostream& err_stream;
        std::istream& in_stream;

        std::map<std::string, bigCSV::csvTable> tables;
        std::string line;
        std::vector<std::string> command;

        void get_command(std::istream& in);

        std::map<std::string, std::string> get_attribute_map(int& index);
        bool modify_attribute_map(int& index, std::map<std::string, std::string>& atts);
        bool set_map_attribute(const std::string &pair, std::map<std::string, std::string> &attributes);

        void create();
        void select();
        void insert();
        void alter();
        void update();

        std::string getNextWord(std::string& line, std::string::iterator& it);

        bool parse_where_clause(int &index, csvTable &table, Conditions &conditions);

        csvTable* get_table(const std::string &name);
    };
}

#endif //BIG_CSV_SHELL_HPP