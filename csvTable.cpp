//
// Created by jakub on 14. 3. 2021.
//

#include "csvTable.hpp"
#include "helper.hpp"
#include "tmpFileFactory.hpp"
#include "RowComparator.hpp"
#include "Conditions.hpp"

#include <vector>
#include <fstream>
#include <iostream>

namespace bigCSV{

    void csvTable::addStream(const std::filesystem::path& path, char delimiter, char endline, char quotechar) {
        csvFile file(path, delimiter, endline, quotechar);
        schema = createJoinedSchema(schema, file.schema);
        input_files.emplace(path, std::move(file));
    }

    void csvTable::printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const std::function<bool(const std::vector<std::string>&)>& condition){
        out<<formatRow(input_columns, delimiter, quotechar, endline);
        for(auto&& file: input_files){
            file.second.printColumns(out, input_columns, condition, out_delimiter, out_quotechar, out_endline);
        }
    }

    bigCSV::csvFile csvTable::merge2(csvFile& first, csvFile& second, const RowComparator& comp) const{

        // Open input files
        first.open_input_stream(true);
        second.open_input_stream(true);

        //std::cout<<"Files opened"<<std::endl;

        // Initialize rows
        auto first_columns = first.getNextTableRow();
        auto second_columns = second.getNextTableRow();

        //std::cout<<"Rows initialized"<<std::endl;

        // Create the table schema
        //std::cout<<"Creating schema"<<std::endl;
        std::vector<std::string> schema = createJoinedSchema(first_columns.schema, second_columns.schema);
        //std::cout<<"Joined schema = "<<formatRow(schema, delimiter, quotechar, endline)<<std::endl;

        // Open output file
        bigCSV::File out_file = tmpFileFactory::get_tmpFile();
        std::ofstream out_stream(out_file.get_path(), std::ofstream::trunc);
        out_stream<<formatRow(schema, delimiter, quotechar, endline);


        // Merge
        while(!first_columns.empty && !second_columns.empty){
            if(comp(first_columns, second_columns)){
                out_stream<<formatRow(first_columns.toLine(schema), delimiter, quotechar, endline);
                first_columns = first.getNextTableRow();
            }
            else{
                out_stream<<formatRow(second_columns.toLine(schema), delimiter, quotechar, endline);
                second_columns = second.getNextTableRow();
            }
        }
        while(!first_columns.empty){
            out_stream<<formatRow(first_columns.toLine(schema), delimiter, quotechar, endline);
            first_columns = first.getNextTableRow();
        }
        while(!second_columns.empty){
            out_stream<<formatRow(second_columns.toLine(schema), delimiter, quotechar, endline);
            second_columns = second.getNextTableRow();
        }

        return csvFile(std::move(out_file), delimiter, endline, quotechar);
    }

    void csvTable::sort(std::ostream& out, const RowComparator &comp) {
        sort(out, comp, tautology, schema);
    }

    void csvTable::sort(std::ostream& out, const RowComparator &comp, const std::function<bool(const std::vector<std::string>&)>& condition, const std::vector<std::string>& columns) {
        std::vector<csvFile> files1;
        std::vector<csvFile> files2;

        // Distribute input_files and save them into files1
        for(auto&& file : input_files){
            auto dist_files = file.second.distribute(condition);
            for(auto&& dist_file : dist_files){
                auto tmp_file = tmpFileFactory::get_tmpFile();
                std::ofstream of(tmp_file.get_path(), std::ofstream::trunc);
                dist_file.trivialSort(of, comp);
                //std::cout<<"printing trivially sorted file"<<std::endl; // Debug
                //dist_file.printColumns(std::cout); // Debug
                files1.emplace_back(std::move(tmp_file), delimiter, endline, quotechar);
            }
        }
        //std::cout<<"Distributed into "<<files1.size()<<" files."<<std::endl;


        std::cout<<"Distributed"<<std::endl;

        // Merge
        auto input_v = &files1;
        auto output_v = &files2;
        while(input_v->size() > 1){
            output_v->clear();
            for(int i=1; i<input_v->size(); i+=2){
                //std::cout<<"Size of input_v = "<<input_v->size()<<std::endl;
                if(i == input_v->size()) {
                    output_v->emplace_back(std::move((*input_v)[i - 1]));
                }
                else{
                    output_v->emplace_back(merge2((*input_v)[i-1],(*input_v)[i],comp));
                    //std::cout<<"\nFILE:"<<std::endl;
                    //output_v->front().printColumns(std::cout);
                }
            }
            //std::cout<<"Size of output = "<<output_v->size()<<std::endl;
            std::swap(output_v, input_v);
        }

        out<<"Merged"<<std::endl;

        (*input_v)[0].printColumns(out,columns,tautology, out_delimiter, out_quotechar, out_endline);
    }

}
