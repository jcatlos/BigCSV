# Testing
For testing purposes, there is the `inputs` folder containing 2 input files and 3 demonstrational scripts:
* `small_input.csv` Contains data small enough to easily test whether all functions work properly
* `covid_trade.csv` Contains data large enough that the sorting is done by the file distribution and merging
* `update_script.in` Contains a short demonstration of the UPDATE statement. Firstly prints data about students. Then sets the 'Grade' to 'PASSED' to all students who achieved more than 50 points in the 'Final' test. This updated table is savet to `out.csv` and then loaded by the script and printed to the screen.
* `max_filesize_comparison_script.in` Demonstrtes the difference of calculation speed when the `MAX_FILESIZE` constant is increased. On a large file (~100000 rows) runs a sorting SELECT statement firstly with MAX_FILESIZE = 13500 (default value) and then MAX_FILESIZE = 100000.
* `multiple_tables_script.in` Demonstrates the ability to use multiple tables in the same script and multiple input files for querying. Using the same table as the `max_filesize_comparison_script.in` filters the rows to those containing China's export data and saves them to `china_exports.csv` with different OUT_DELIMITER and OUT_QUOTECHAR. Then does the same for China's imports and saves those rows to `china-imports.csv`. Then a new table `china-info` is created. Its IN_DELIMITER and IN_QUOTECHAR are changed to those used to exporting the rows. Then the 2 files are added to the `china-info` table and the table is printed out.

Currently, the scripts use path that works with launching the  program in Visual Studio 19 (where the program runs in `catlos/build/` directory). If testing in other environment, change the paths in all `INSERT INTO` statements. Regular slashes as directory delimiters should be used **even on Windows**.

To execute the string in the utility use:
```
./BigCSV < ../inputs/script.in
```
However in this command use directory separators **according to the target OS**.