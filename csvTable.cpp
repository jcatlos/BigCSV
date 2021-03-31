//
// Created by jakub on 14. 3. 2021.
//

#include "csvTable.hpp"
#include "helper.hpp"
#include "tmpFileFactory.hpp"
#include "RowComparator.hpp"

#include <vector>
#include <fstream>
#include <iostream>

namespace bigCSV{

    // REMOVE THE EXTENDS?
    void csvTable::addStream(const std::filesystem::path& path, char delimiter, char endline, char quotechar) {
        csvFile file(path, delimiter, endline, quotechar);
        input_files.emplace(path, std::move(file));
    }

    // FOR NOW ONLY WORKS ON CONSOLE OUTPUT
    void csvTable::printColumns(std::ostream& out, const std::vector<std::string>& input_columns){
        std::cout<<formatRow(input_columns, delimiter, quotechar, endline);
        for(auto&& file: input_files){
            file.second.printColumns(out, input_columns);
        }
    }

    bigCSV::csvFile csvTable::merge2(csvFile& first, csvFile& second, const RowComparator& comp) const{
        // Open input files
        first.open_input_stream(true);
        second.open_input_stream(true);

        // Initialize rows
        auto first_columns = first.getNextTableRow();
        auto second_columns = second.getNextTableRow();

        // Create the table schema
        std::vector<std::string> schema = createJoinedSchema(first_columns, second_columns);

        // Open output file
        bigCSV::File out_file = tmpFileFactory::get_tmpFile();
        std::ofstream out_stream(out_file.get_path(), std::ofstream::trunc);
        out_stream<<formatRow(schema, delimiter, quotechar, endline);


        // Merge
        while(first.open && second.open){
            if(comp(first_columns, second_columns)){
                out_stream<<formatRow(first_columns.toLine(schema), delimiter, quotechar, endline);
                first_columns = first.getNextTableRow();
            }
            else{
                out_stream<<formatRow(second_columns.toLine(schema), delimiter, quotechar, endline);
                second_columns = second.getNextTableRow();
            }
        }
        while(first.open){
            out_stream<<formatRow(first_columns.toLine(schema), delimiter, quotechar, endline);
            first_columns = first.getNextTableRow();
        }
        while(second.open){
            out_stream<<formatRow(second_columns.toLine(schema), delimiter, quotechar, endline);
            second_columns = second.getNextTableRow();
        }

        return csvFile(std::move(out_file), delimiter, quotechar, endline);
    }

    void csvTable::sort(std::ostream& out, const RowComparator &comp) {
        std::vector<csvFile> files1;
        std::vector<csvFile> files2;

        // Distribute input_files and save them into files1
        for(auto&& file : input_files){
            auto dist_files = file.second.distribute();
            for(auto&& dist_file : dist_files){
                auto tmp_file = tmpFileFactory::get_tmpFile();
                std::ofstream of(tmp_file.get_path());
                dist_file.trivialSort(of, comp);
                files1.emplace_back(std::move(tmp_file), delimiter, quotechar, endline);
            }
        }

        std::vector<std::string> a;
        a.push_back("First name");
        files1[0].printColumns(std::cout, a);

        std::cout<<"Distributed"<<std::endl;

        // Merge
        auto input_v = &files1;
        auto output_v = &files2;
        while(input_v->size() > 1){
            output_v->clear();
            for(int i=1; i<input_v->size(); i+=2){
                if(i == input_v->size()) {
                    output_v->emplace_back(std::move((*input_v)[i - 1]));
                }
                else{
                    output_v->emplace_back(merge2((*input_v)[i-1],(*input_v)[i],comp));
                }
            }
            auto help_v = input_v;
            input_v = output_v;
            output_v = help_v;
        }

        out<<"Merged"<<std::endl;

        (*input_v)[0].printColumns(out);
    }

}
