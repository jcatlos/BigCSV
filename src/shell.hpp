#ifndef BIGCSV_SHELL_HPP
#define BIGCSV_SHELL_HPP

#include <fstream>
#include <iostream>
#include <map>

#include "csvTable.hpp"

namespace bigCSV {

    class Shell{
    public:
        Shell(std::istream& in, std::ostream& err) : err_stream(err), in_stream(in) {}
        void run();
    private:
        // Streams
        std::ostream& err_stream;
        std::istream& in_stream;

        // Map of created tables and a retrieval method
        std::map<std::string, csvTable> tables;
        csvTable* get_table(const std::string &name);
        
        // Currently parsed command in string and vector of token representation
        std::string command_line;
        std::vector<std::string> command;

        // Functions for command parsing
        void get_command(std::istream& in);
        std::string getNextWord(std::string& line, std::string::iterator& it) const;

        // Attribute map functions
        std::map<std::string, std::string> get_attribute_map(int& index) const;
        bool modify_attribute_map(int& index, std::map<std::string, std::string>& atts) const;
        bool set_map_attribute(const std::string &pair, std::map<std::string, std::string> &attributes) const;

        // Functions for commands - only for better readibility
        void create();
        void select();
        void insert();
        void alter();
        void update();

        // Where clause parser
        bool parse_where_clause(int &index, csvTable &table, Conditions &conditions);

    };
}

#endif
