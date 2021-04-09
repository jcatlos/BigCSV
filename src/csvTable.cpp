#include "csvTable.hpp"
#include "helper.hpp"
#include "tmpFileFactory.hpp"
#include "RowComparator.hpp"
#include "Conditions.hpp"

#include <vector>
#include <fstream>
#include <iostream>

namespace bigCSV{

    // Adds a file into the table
        // No file is modified while in the table - all changes are performed on copies of the files
    void csvTable::addFile(const std::filesystem::path& path, char delimiter, char endline, char quotechar) {
        csvFile file(path, delimiter, endline, quotechar);      // Create a csvFile instance
        file.init_file();                                       // Initialize the variables
        schema = createJoinedSchema(schema, file.schema);       // Update the schema of the table
        input_files.emplace(path, std::move(file));             // As the csvFile has deleted copy-constructor, the file is moved into the input_files
    }

    // From each file in the table selects all columns from input_columns from rows that satisfy the 'condition' into the 'out' ostream
        // Table's out characters are used for the printing
    void csvTable::printColumns(std::ostream& out, const std::vector<std::string>& input_columns, const Conditions& conditions){
        // Print out the header for the output
        out<<formatRow(input_columns, out_delimiter, out_quotechar, out_endline);
        for(auto&& file: input_files){
            // Call printColumns for all files in the table
            file.second.printColumns(out, input_columns, conditions, out_delimiter, out_quotechar, out_endline);
        }
    }

    // Given 2 sorted files, returns a sorted file containing the rows of the 2 files
    bigCSV::csvFile csvTable::merge2(csvFile& first, csvFile& second, const RowComparator& comp) const{
        int out_rows = 0;
        // Open input files
        first.open_input_stream();
        second.open_input_stream();

        // Initialize rows
        auto first_columns = first.getNextTableRow();
        auto second_columns = second.getNextTableRow();

        // Create the table schema
        std::vector<std::string> schema = createJoinedSchema(first_columns.schema, second_columns.schema);

        // Open output file
        bigCSV::File out_file = tmpFileFactory::get_tmpFile();
        std::ofstream out_stream(out_file.get_path(), std::ofstream::trunc);
        out_stream<<formatRow(schema, in_delimiter, in_quotechar, in_endline);


        // Merge
        while(!first_columns.empty && !second_columns.empty){
            if(comp(first_columns, second_columns)){
                out_rows++;
                out_stream<<formatRow(first_columns.toLine(schema), in_delimiter, in_quotechar, in_endline);
                first_columns = first.getNextTableRow();
            }
            else{
                out_rows++;
                out_stream<<formatRow(second_columns.toLine(schema), in_delimiter, in_quotechar, in_endline);
                second_columns = second.getNextTableRow();
            }
        }
        while(!first_columns.empty){
            out_rows++;
            out_stream<<formatRow(first_columns.toLine(schema), in_delimiter, in_quotechar, in_endline);
            first_columns = first.getNextTableRow();
        }
        while(!second_columns.empty){
            out_rows++;
            out_stream<<formatRow(second_columns.toLine(schema), in_delimiter, in_quotechar, in_endline);
            second_columns = second.getNextTableRow();
        }

        first.close_input_stream();
        second.close_input_stream();
        return csvFile(std::move(out_file), in_delimiter, in_endline, in_quotechar);
    }

    // Shorter overload for the sort function
    void csvTable::sort(std::ostream& out, const RowComparator &comp) {
        Conditions conds;
        sort(out, comp, conds, schema);
    }

    // MergeSort on all files in input_files
        // Outputs to the out ostream
        // Compares rows based on provided RowComparator
        // Filters the rows based on provided condition
        // Selects only the provided columns
    void csvTable::sort(std::ostream& out, const RowComparator &comp, const Conditions& conditions, const std::vector<std::string>& columns) {
        // Using 2 vectors of files - In each iteration, files from one are merged and placed into second
        std::vector<csvFile> files1;
        std::vector<csvFile> files2;

        // Firstly, divide the input files into small enough fiels to be sorted in-memory and sort them
        for(auto&& file : input_files){
            auto dist_files = file.second.distribute(conditions, columns, max_filesize);
            for(auto&& dist_file : dist_files){
                auto tmp_file = tmpFileFactory::get_tmpFile();
                std::ofstream of(tmp_file.get_path(), std::ofstream::trunc);
                dist_file.trivialSort(of, comp);
                files1.emplace_back(std::move(tmp_file), dist_file.delimiter, dist_file.endline, dist_file.quotechar);
            }
        }
        std::cerr<<"Distributed into "<<files1.size()<<" files."<<std::endl;


        // "Labels" for the file vectors - swapped after each iteration
            // Prevents extra copying
        auto input_v = &files1;
        auto output_v = &files2;

        // Merge files from input_v to output_v until there is ony one left
        while(input_v->size() > 1){
            //std::cout<<"Input size = "<<input_v->size()<<std::endl;
            output_v->clear();
            for(int i=1; i<=input_v->size(); i+=2){
                // The last file (if the number of files is odd) is just placed to the output_v
                if(i == input_v->size()) {
                    output_v->emplace_back(std::move((*input_v)[input_v->size() - 1]));
                }
                else{
                    output_v->emplace_back(merge2((*input_v)[i-1],(*input_v)[i],comp));
                }
            }
            std::swap(output_v, input_v);
        }
        //std::cout<<"Input size = "<<input_v->size()<<std::endl;


        // There is only one file in the input_v - means everythng is sorted
            // Firstly, header has to be added
        out<<formatRow(columns, out_delimiter, out_quotechar, out_endline);
        Conditions conds;         // Empty list of conditions
        (*input_v)[0].printColumns(out,columns, conds, out_delimiter, out_quotechar, out_endline);
    }

    // Each file in the table is updated by the RowUpdate if it satisfies the condition
        // Temporary copies of files are created to prevent actual file modification
    void csvTable::updateTable(const Conditions &conditions, BigCSV::RowUpdate &update) {
        std::map<std::filesystem::path, csvFile> new_files;
        for(auto&& file: input_files){
            csvFile new_file = file.second.createUpdatedFile(conditions, update);
            new_file.init_file();
            std::filesystem::path file_path = new_file.get_path();
            new_files.emplace(std::make_pair(file_path, std::move(new_file)));
        }
        input_files = std::move(new_files);
    }

}
