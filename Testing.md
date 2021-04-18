# Testing
For testing purposes, there is the `inputs` folder containing 2 input files and 3 demonstrational scripts:
* `small_input.csv` Contains data small enough to easily test whether all functions work properly
* `covid_trade.csv` Contains data large enough that the sorting is done by the file distribution and merging
* `small_script.in` Contains a short demonstration of how a script for the utility can be written.
* `max_filesize_comparison_script,in` Demonstrtes the difference of calculation speed when the `MAX_FILESIZE` constant is increased.
* `join_tables_script.in` Demonstrates the ability to use multiple input files for querying.

Currently, the scripts use path that works with launching the  program in Visual Studio 19 (where the program runs in `catlos/build/` directory). If testing in other environment, change the paths in all `INSERT INTO` statements. Regular slashes as directory delimiters should be used **even on Windows**.

To execute the string in the utility use:
```
./BigCSV < ../inputs/script.in
```
However in this command use directory separators **according to the target OS**.