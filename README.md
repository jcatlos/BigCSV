# BigCSV .csv file editor

## Installation
Make sure, you have installed and running **cmake (3.15 or newer)** and a compiler supporting **C++ 17**.

### Linux
Open the cloned repository and run these commands in your terminal:

``` shell
rm -r build         # To make sure, the build folder is not present
mkdir build
cd build
cmake ..
cmake --build .
```
This approach may work on Windows, as well, however it has not been tested
### Windows
Use cmake GUI application

Afterwards, the program is ready to be used. The executable is located in the `build` folder

## Usage
The utility takes arguements via the standrd input. To create and use a script, just write it down to a text file and launch the utility by `BigCSV < [file_name]`.

Command are always ended by a semicolon `;`. All tokens are divided by any whitespace. A token containing a whitespoace must be inside double quotes `"`.

If an error is encountered while parsing a command, no changes to the tables happen.

**Note:**
All tables are stored only in memory. Therefore any modifications performed by changing output attributes or using the `UPDATE` are not saved. If you wish to save them, use the `SELECT` command with the `INTO` clause to save the chnges into a file which can be then loaded as a separate table.

## Commands
There are 6 commands available: `CREATE TABLE` for creating a table to work with, `ALTER TABLE` to modify its attributes, `INSERT` to add source files into a table, `UPDATE` to modify data in a table, `SELECT` to display data currently in a table and `EXIT` to exit the utility. 


### CREATE TABLE
**Syntax:** `CREATE TABLE table_name [SET att=val, … ];`

Creates a table named `table_name`. If a value of an attribute is not provided, a default value is used. Attributes and their default values are explained in the [Attributes section](Attributes).

### ALTER TABLE
**Syntax:** `ALTER TABLE table_name [SET att=val, … ];`

Alters an existing table with name `table_name` by modifying its [attributes](Attributes). Previous values of attributes are taken as the default values.

### INSERT
**Syntax:** `INSERT INTO table_name FILE path [DELIMITER=delim] [ENDLINE=endl] [QUOTECHAR=qchar]`

Adds a file to an existing table `table_name` as a source of data. **In no moment is the soucre file modified.** `DELIMITER`, `ENDLINE` and `QUOTECHAR` signify the delimiter of the tokens, the delimiter of the rows and th character used to optionally encapsulate tokens of the file. If none given, the values of `IN_DELIMITER`, `IN_ENDLINE` and `IN_QUOTECHAR` of the table `table_name` are used.

### UPDATE
**Syntax:** `UPDATE table_name SET col1=val1,… [WHERE cond, ...];`

Modifies the values in the existing table `table_name` by setting the specified columns to specified values. This modification can be restricted by the `WHERE` clause. Then only the rows fullfilling all conditions in the clause are modified. In the process of the modification, all the source files are copied into [temporary files]. 

### SELECT
**Syntax: ** `SELECT col1, …, coln FROM table_name [WHERE cond] [ORDER BY col1 … ] [INTO path];`

Prints the columns specified by their names in the source files from an existing table `table_name`. These results may be filtered by the `WHERE` clause and ordered by the `ORDER BY` clause. If a `INTO` clause is present, the output is instead saved into provided relative path `path`. Errors and warnings are still printed to the standard error output.

The columns of the `ORDER BY` clause *must* be a subset of the selected columns.
By using the `ORDER BY` clause, the sorting process is started. The files are distributed into temporary partition files of [`MAX_FILESIZE`](), which are sorted in memory and then merged into one file.



