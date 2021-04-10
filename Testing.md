# Testing
For testing purposes, there is the `inputs` folder containing 3 files:
* `small_input.csv` Contains data small enough to easily test whether all functions work properly
* `large_input.csv` Contains data large enough that the sorting is done by the file distribution and merging
* `script.in` Contains a short demonstration of how a script for the utility can be written.

The files can be accessed from the utility by using the path `../inputs/small_input.csv`. Regular slashes should be used **even on Windows**.

To execute the string in the utility use:
```
./BigCSV < ../inputs/script.in
```
However here use directory separators **according to the target OS**.