# BigCSV: Technical documentation
This document serves to depict the internal workings of the BigCSV command line utility, as well as the whole BigCSV namespace. 

> **Disclaimer:** For better readibility, all function signatures are written without the `std::` namespace specifiers

## Structure
All of the code is located in `BigCSV` namespace. Each class is declared in a single `.hpp` file and defined in a `.cpp` file of the same name. Then there are `main.cpp` file serving to launch an instance of `Shell` and `helper.hpp` and `helper.cpp` containing functions not associated to any class. The program can be logically divided into 2 independent parts: the Database and the Shell. The functions in the [`Helper class`](#Helper) are used by both.

### Shell
The shell proivides an interface of a command line utility to use the functionality of the BigCSV namespace. It is responsible for accepting and parsing user input and executing appropriate functions of the BigCSV namespace. In the following sections, its inner structure is explained. For user manual, please refer to the *User Manual*.

#### Attributes
* `private istream& in_stream` : The input stream from which the commands are accepted
* `private ostream& err_stream` : The output stream for error messages 
* `private map<string, csvTable> tables` : A map containing the user defined tables by their names (as specified by the user)
* `private string command_line` : The last entered command stored as string
* `private vector<string> command` : The last entered command in tokenized form

#### Constructors
1. `Shell(istream& in, ostream& err)` : Initializes the the `in_stream` and `err_stream` attributes by the provided streams. 

#### get_table()
**Signature:** `csvTable* get_table(const string &name)`

Provided a name of a table, returns an observer pointer to a `csvTable` with that name. If not found, prints out an error message to the `err_stream` and returns a `nullptr`.

#### Functions for command parsing
Following 2 functions are used from extracting commands from an input stream. After calling `get_command()`, attributes `command` and `command_line` are updated to the next command in the stream.
##### getextWord()
**Signature:** `string getNextWord(string& line, string::iterator& it) const`

Given a current line and an iterator pointing to the beginning of a word, returns the next word and moves the iterator to the end of the word. If the first character is a `"`, it is processed until the next non-escaped `"`. Escaping is done using the `\` (backslash). 

##### get_command()
**Signature:** `void get_command(istream& in)`

Gets the next command from the istream and modifies the `Shell` attributes accordingly. Commands are delimited by `;` (semicolon). Uses `std::getline()` with delimiter set to ';' (semicolon) to retrieve the command as a string, which is saved to the `command_line` attribute and divided it into tokens using the `getNextWord()` method. The use of semicolon as line delimiter disallows the usage of the character in the shell commands, as escaping of characters is handled after the line is extracted from the stream. 

A commant prompt is displayed to the user if the utility is used in the terminal. The check is performed by comparison `&in == &std::cout`.

#### Attribute map functions
Following 2 functions are used to parse the `SET` clauses of commands by maintaining and updating a map (attribute name -> attribute value) handled accross these functions.

##### modify_attribute_map()
**Signature:** `bool modify_attribute_map(int& index, map<string, string>& atts) const`

Given an index into the `command` and a map of attributes to modify, parses the `SET` clause of command. That is done by traversing through the command, looking for sequences of tokens in form `attribute_name`, `=`, `value`. Returns a boolean signalizing whether any problems arose (if `attribute_name` is not a name of an attribute or `attribute_value` is empty) during the process. In such case, the reason of the problem is sent onto the `err_stream`. When the second token of a looked sequence is not `=` returns `true`. In both cases, the `index` is now the index after the last valid sequence. 

If a value for an attribute is specified in multiple `attribute = value` sequences, the last pair is used.

##### get_attribute_map()
**Signature:** `map<string, string> get_attribute_map(int& index) const`

Creates a map of the attributes of a table with default values and calls `modify_attribute_map()` on it. If the function returns `false`, an empty map is returned.

#### parse_where_clause()
**Signature:** `bool parse_where_clause(int &index, csvTable &table, Conditions &conditions)`

This function is responsible for parsing the `WHERE` clause of a command. It checks the pressence of the `WHERE` token and tries to extract as many conditions as possible. Each condition must be in the form of `column operator value`, where operator is one of the `=`, `<` or `>`. Then a `Conditions` object is constructed using the conditions in the order in which they were specified on the input as follows:
* `=` Adds a `Conditions::AddEquals()` condition
* `<` Adds a `Conditions::AddIntLt()` condition
* `>` Adds a `Conditions::AddIntGt()` condition

Returns a boolean signifying whether the parsing went well. Any errors are output in the `err_stream` and `false` is returned. 

#### Command processing functions
The following functions are responsible for parsing and execution of the commands to the shell. They check the syntax and handle all accesses to the underlying data structures. They have no parameters nor return values.

No modifications to the `Shell` instance or the underlying tables are done if an ill-formated command is entered.
 
##### create()
Is responsible for the execution of `CREATE` command. Checks the form of the command and processes the `SET` clause by calling `get_attribute_map()`. If a valid attribute map is returned, the `MAX_FILESIZE` attribute is checked to be a non-negative integer (uses `std::stoi()` for conversion to int). Then adds a table with the specified name to `Shell::tables` map.

##### select()
Is responsible for processing of the `SELECT` command. Creates a vector of column names to be printed by appending all tokens between the `SELECT` and `FROM` tokens. If the end of the command is encountered, an error is thrown, as the mandatory `FROM` clause is not present. Finds the specified table using `get_table()`. Then checks for a `WHERE` clause in which case calls `parse_where_clause()`. Then checks for an `ORDER BY` clause and if present, creates a vector of column names (until the `INTO` token or the end of the command are found) to be sorted by and checks whether they are a subset of the columns in the `SELECT` clause (this condition is required by the `csvTable::sort()` function). Then checks for an `INTO` clause. If present, the output stream is changed to a ofstream into the selected file. If there are any more tokens present in the command, an error is thrown.

Finally, depending on the pressence of the `ORDER BY` clause, the `csvTable::printColumns()` of `csvTable::sort()` function is called.

##### insert()
Is responsible for processing of the `CREATE` command. Checks the syntax of the command, then checks for the validity of the provided name using `get_table()`. If the command contains more tokens, `get_attribute_map()` is called to extract any attributes provided. If the function finishes successfully, `csvTable::addFile()` is called using the `IN_...` attributes. 

##### alter()
Is responsible for processing of the `ALTER` command. Checks the syntax of the command and finds the specified table using `get_table()`. Creates a map of attributes which is passed to the `modify_attribute_map()` function (this is the reason that the `modify_attribute_map()` function is kept separate from the `get_attribute_map()`). Then tries to use `std::stoi()` checks for the non-negativity of the `MAX_FILESIZE` attribute and if it is valid sets the table's attributes to correspond to the demanded values.

##### update()
Is responsible for processing of the `UPDATE` command. First checks for the syntax of the command and the existence of the table by `get_table()`. Then extracts the pair `column=value` and splits it by the `=` character. If the pair is malformed, throws an error. The existence of the column is tested and an according `RowUpdate` object is constructed. An error is thrown if the column does not exist in the table. The potential `WHERE` clause is parsed by calling the `parse_where_clause()` and `csvTable::update` with the constructed `RowUpdate` and the `Conditions` is called. 

#### run() function
This is the only public function of the `Shell` class serving to launch an intance of the shell. If the shell's `in_stream` is `std::cout` a description of the utility is shown. That is followed by an infinite loop of reading commands from the `in_stream`. When a command is read, an appropriate command function is called to process it. The utility is terminated by the `EXIT;` command. Other commands are determined by its first word (`command[0]`) and if the command is not recognizer, an error is passed to the `err_stream`.

### Database

#### csvTable
The `csvTable` represents a 'table' of csv files. It manages its instances of [csvFile](#csvFile) and is the interface for access to the data. The table itself **does not** manipulate any data of the files, instead it creates a copy of the file and modifies the copy.
##### Attributes
* `public char in_delimiter` : Default value for the token delimiting character of the inserted files.
* `public char in_quotechar` :  Default value for the token encapsulation character of the inserted files.
* `public char in_endline` :  Default value for the line delimiting character of the inserted files.
* `public char out_delimiter` : The token delimiting character of generated output.
* `public char out_quotechar` : The token enclosing character of generated output.
* `public char out_endline` : The line delimiting character of generated output.
* `public int max_filesize` : The 'maximum' amount of characters a file that can be sorted in memory may hold. In reality all files contain slightly more, explained [here](#distribute).
* `public vector<string> schema` : The names of columns of all tables in the table. 
* `private map<filesystem::path, csvFile> input_files` : A map from file path to a csvFile. Ensures the table does not contain multiple copies of the same file. 
##### Constructors
* **Signature:** `csvTable(char delim, char qchar, char endl, int size)` 

Initializes the `in_*` and `out_*` variables to the provied values and the `max_filesize` to `size`.

* **Signature:** `csvTable()`

Initializes all variables to the default values: 
* Delimiters to `;`
* Quotechars to `"`
* Endlines to `\n`
* Max filesize to `13500` (This value was chosen as 'safe' during devlopment and probably can be tweaked much higher - not tested, but `50000` was all right on my machine (16GB RAM))

##### Functions
###### printColumns()
**Signature:** `public void printColumns(ostream& out, const vector<string>& input_columns, const Conditions& conditions)`

Prints the columns specified by `input_columns` on the rows satisfying `conditions` into the `out`. This is done by calling the `csvFile::printColumns()` method of all files in the table with the same parameters.

###### sort()
**Signature 1:** `public void sort(ostream& out, const RowComparator& comp)`
**Signature 2:** `public void sort(ostream& out, const RowComparator& comp, const Conditions& conditions, const vector<string>& columns)`

The function performs a modified version of the mergesort algorithm (sorted by `comp`) on all rows satisfying the `conditions`. Columns specified by their anme in `columns` are printed to the `out`. 
> **Note:** The column indxes of the `RowComparator` must correspond to the indexes in the `sorted_columns`, not the table's `schema`.

The shorter overload works uses a default condition (always `true`) and prints all columns of the table.

The sorting process goes as follows:
1. `csvFile::distribute()` with provided restrictions is called on each file of the table and the resulting temporary files are saved into a list
2. While the resulting list contains more than one `csfFile`, do:
    1. Create a new vector of `csvFile`s
    2. Merge each odd `csvFile` with the following `csvFile` (using  [`merge2()`](#merge2())) and add the resulting `csvFile` to the created vector
    3. If the last `csvFile` does not have an even `csvFile` to be merged with, just copy it to the created vector
3. When the list contains only one `csvFile`, the file contains all relevant rows and can be printed to the `out`

> **Note:** The actual implementation does not create new lists, and instead uses 2 vectors - one source and one output, which are swapped after each iteration.

> **Note:** Decreasing the number of columns and restricting the query results by `Conditions` is advised as the distributed files contain only relevant data. This reduces the amount of files required and therefore the query is evaluated much faster.

###### addFile()
**Signature:** `public  void addFile(const filesystem::path& path, char delimiter, char endline, char quotechar)`

Given a file path and the special characters of a file, adds the file at the `path` with the characters into the `input_files`. The `in_delimiter`, `in_quotechar` and `in_endline` variables should be used by default. The file'sexistence is not controlled, however it does not affect any of the queries. 
       
###### updateTable()
**Signature:** `public void updateTable(const Conditions &conditions, RowUpdate &update)`

Calls the `csvFile::createUpdatedFile()` with the `conditions` and `update` on each file in `input_files`. The results are stored in a new map which at the end takes the place of the `input_files`. This approach prevents any modification of the source files.

###### merge2()
**Signature:** `private csvFile merge2(csvFile &first, csvFile &second, const RowComparator &comp) const`

Given a `RowComparator` and 2 `csvFile`s holding a `File` with rows sorted by the `RowComparator` returns a new `csvFile` holding a temporary `File` with rows from both `csvFile`s sorted by the `RowComparator`. This is done firstly by creating a joined schema from both `csvFiles` (they may contain differrent columns) and then applying the mergesort approach: both files are read line by line and the new file is constructed by selecting the 'smaller' row in each step. This approach was selected as the files do not have to be fully loaded into the memory to be sorted. 

#### csvFile
This class provides an iterface for reading and manipulation of a file as a csv table. 
##### Attributes
* `private File file` : Is the source file       
* `private char delimiter` : Is the delimiter character of the source file
* `private char endline` : Is the endline character of the source file
* `private char quotechar` : Is the quoting character of the source file
* `private ifstream input_stream` : Is an internal stream for reading the source file
* `public vector<string> schema` : A vector of column names in the souce file
* `public map<string, int> columns` : An inverted index for the `schema`

##### Constructors
**Signature 1:** `csvFile(const filesystem::path& fn, char delim, char le, char q)`
**Signature 2:** csvFile(File&& fn, char delim, char le, char q);

Both constructors initialize the `csvFile` by the provided variables. The first takes a reference to a `filesystem::path` which is used to construct a new `File` instance, whereas the second constructor takes an rvalue reference to an already created instance which is `std::move()`-d.

##### Functions
###### get_path()
**Signature:** `public filesystem::path get_path() const`

Returns the path to the source file.

###### open_input_stream() and close_input_stream()
**Open signature:** `public void open_input_stream()`
**Close signature:** `public void close_input_stream()`

`open_input_stream()` opens the `input_stream` at the beginning of the `file`. Then the first (header) row of the file is read and processed to initialize the attributes `schema` and `columns`.

`close_input_stream()` only closes the file.

These 2 functions should be called before and after any reading from the source file. Their existence allows for the `input_stream` to be private to protect it from any unwanted modifications.

###### init_file()
**Signature:** `public inline void init_file()`

Uses `open_file_stream()` followed by `close_file_stream()`. Used to initialize the `schema` and `columns` variables withaout wanting to read the contents of the file.

###### not_eof()
**Signature:** `public inline bool not_eof()`

Checks the `good()` bit on the `input_stream` and whether the next character is not `EOF`. That happens after reading the last line of the file - the next file would be empty. This check prevents this case from happening.

###### getNextLine()
**Signature:** `public vector<:string> getNextLine()`

Returns the next line from the stream as a vector of values of the columns. Quoted values are parsed using `helper::getQuotedString()`. All leading and trailing whitespace is removed, unless it is inside the quotes.

###### getNextTableRow()
**Signature:** `public TableRow getNextTableRow()`

Returns the next line from the stream as a `TableRow`. Done by calling the `getNextLine()` and using the `schema` it is converted to the `TableRow` form.

###### printColumns()
**Signature 1:** `public void printColumns(ostream& out, const vector<string>& input_columns, const Conditions& conditions, char out_delimiter, char out_quotechar, char out_endline)`

**Signature 2:** `public void printColumns(ostream& out, const vector<string>& input_columns)`

**Signture 3:** `public void printColumns(ostream& out)`

All 3 overloads serve for prining of the rows to the specified `ostream`. The `create_mask()` function is called to efectively process the values on the rows. Then each row is read by `getNextLine()` and checked against the `conditions`. Then the values of `input_columns` are extracted and passed to `formatRow()` alongside the `out_delimiter`, `out_quotechar` and `out_endline` characters and printed to the `out` stream.

The shorter overloads set `input_columns` to `schema`, `conditions` to `Conditions()` (no conditions) and `out_*` characters to the `delimiter`, `quotechar` and `endline` attributes.

###### trivialSort()
**Signature:** `public void trivialSort(ostream& out, const RowComparator& comp)`

Reads all lines of the source file by `getNextTableRow()` into a vector. This vector is then sorted by `std::sort` using the `comp` RowComparator. Then the `schema` and the content of the vector are printed into the `out` stream using the `formatRow()` method. All special character for the `formatRow()` are inherited from the `csvFile` instance.

###### distribute()
**Signature 1:** `public vector<csvFile> distribute(const Conditions &conditions, const vector<string> &input_columns, size_t max_filesize)`

**Signature 2:** `public vector<csvFile> distribute(const Conditions& condition, size_t max_filesize)`

Given the `conditions`, `input_columns` and `max_filesize`, distributes the source file into multiple temporary files. Only rows satisfying the `conditions` are copied. Each file is filled until its character count exceeds `max_filesize` after inserting a row. Although counterintuitive, this ensures that the files are distributed by rows and therefore their order does not matter in further processing of the temporary files and prevents odd scenarios when the `max_filesize` is too small for a row to fit it. 

Furthermore, the size of the header row is not counted into the character count as it is always processed and skipped before `trivialSort()` and therefore does not affect the size of the sorted vector.

The first overload allows for a selecting the columns added to the temporary files. This allows to insert more rows to the file, as unimportant columns are not copied, decreasing the output file size.
    
##### createUpdatedFile()
**Signature:** `public csvFile createUpdatedFile(const Conditions& conditions, RowUpdate& update)`

Creates a new `csvFile` which has a new temporary file as the source file. This source file contains all rows from the original file, however rows satisfying the `conditions` are replaced by the specified `update` of the row.


###### create_mask()        
**Signature:** `vector<int> create_mask(const vector<std::string> &input_columns)`

Given the `input_columns` creates an `out` vector of indexes as follows:
* If `input_columns[i]` is the name of k-th column in the `csvTable` for some `k`, `out[i] = k`
* Otherwise, the column is not in the table and `out[i] = INT32_MAX`

When using the mask, the program has to check if the indexes from `out` are smaller than the `schema.size()`.


###### Reading a file
Firstly, the `open_input_stream()` must be called to open the private `input_stream` and initialize the `schema` and `columns` which aer crucial for proper file reading. While `not_eof() == true`, the fuctions `getNextLine()` and `getNextTableRow()` may be called. When the file reading is finished, `close_input_stream()` must be called to close the `input_stream` properly.

#### File
Is a class that manages a file. Semantically, there should be a one-to-one relationship betweena a physical file and a `File` instance, therefore the copy-constructor is deleted as well as the empty-constructor. Move-constructor is implemented as it does not affect this rule. 
##### Attributes
* `private filesystem::path path` : The path to the managed file. The path must not be modified, however the `get_path()` function retuns its copy.
* `private bool temporary` : Signifies,m whether the file is a temporary file. If the file is temporary, it is deleted upon the destruction of the `File` instance managing it.
##### Constructors
* `File(filesystem::path p, bool temp)`
* `explicit File(filesystem::path p)`  
* `File(const File& file) = delete` 
* `File(File&& file) noexcept`
##### Destructor
If the source file exists and the `temporary` bit is set, the file is deleted.
##### `const filesystem::path& get_path() const`
Returns a const reference of the `path` to prevent from its modification.

#### TmpFileFactory
Is a singleton object managing the creation of unique temporary files for the table. Has an internal count of created files which is used to create unique names for the files. 
##### Attributes
* `size_t _file_count` : The counter of creted temporary files
* `filesystem::path _tmp_dir` : The path to the system's folder to temporary files
##### Functions
###### _get_file_impl()
**Signature:** `private File _get_file_impl()`  

Returns `File` with the `temporary` bit set to true, so the file will be deleted when the program ends. The file name is constructed as `[_tmp_dir]/tmp_file_[_file_count].csv`, then the `_file_count is incremented.

#### TableRow
Is a class for representing a data from a row in a structured way. This allows to query for values of specific columns of the row more effectively. All attributes and methods are public. The implementation is located in the `TableRow.hpp` and `TableRow.cpp` files.
##### Attributes
* `vector<string> schema` : Vector of names of the columns in the row.
* `vector<string> values` : Vector of values of the columns. They are in the same order as in schema
* `map<string, string> map` : A map from column names to their values
* `bool empty` : Whethwer the `TableRow` is valid
##### Constructors
Has only the implicit constructor (). variable `empty` is by default set to true and it has to be changed if the row is modified manually (does not apply when using `add()`).
##### Functions
###### add()
**Signature:** `void add(const string& col_name, const string& value);`
Given a column's name and its value, adds this pair into its internal data structures. If the `col_name` is already present, its value is modified.

###### toLine()
**Signature:** `vector<string> toLine(const vector<string>& input_schema) const;`
Returns a vector of string of the values in the order specified by `input_schema`. If a value from the `input_schema` is not present in the row, an empty string is in its position to ensure that each value is in its corresponding place on the line.

#### RowUpdate
This class contains a sequence of updates to columns to be performed on a row. Each column can be updated only once by a `RowUpdate`, providing an update to an existing column overwrites the previous one. 
##### Attributes
* `map<size_t, function<string(const string&)>> rowMap` : A map from an index of the column to be edited to a function that returns a modification of the value. The current implementation disallows multiple updates to the same index. May be modified to be a vector of such functions in the future.
##### Constructors
Has only the implicit constructor `RowUpdate()` containing no updates. An empty update returns a copy of the provided row.
##### Functions
###### apply()
**Signature:** `public vector<string> apply(const vector<string>& input_row) const`

Returns a copy of the `input_row` with all updates applied. If no updates are present, returns a copy of the `input_row`.

###### addChange()
**Signature 1:** `public void addChange(size_t index, const function<string(const string&)>& function);`

**Signature 2:** `public void addChange(size_t index, function<string(const string&)>&& function)`

Given in `index` and `function` adds the `function` to the `rowMap[index]`. It will be applied when `apply()` is called. Adding a `function` to an index already in the `rowMap`, overwrites the previous function. The 2 overloads differ only in the type of reference to the provided `function`.

###### ChangeTo()
So far the only change pre-implemented consisting of 2 methods: 
* `private static string _changeToImpl(const string& from, string to)` : Only returns `to`
* `public static function<string(const string&)> ChangeTo(string to)` : Returns a function created by binding the `to` parameter of the `_changeToImpl()`. This results in a function that modifies any input to the vlaue of `to`.

#### RowComparator
A comprator functor accepting 2 rows and sorting them by an internal order of importance of columns.
##### Constructors
**Signaature:** `explicit RowComparator(vector<string>&& o)`

Uses `std::move()` to initialize the `order`.

The implicit constructor is deleted, as there are no means of modifying the order and a comparator without the order would be useless.
##### Attributes
* `private vector<string> order` : A vector of column names in the order of their impoortance
##### Operator()
Is the only function of the class.

**Signature:** `public bool operator()(const TableRow& first_row, const TableRow& second_row) const;`

Given the 2 `TableRow`s traverses the internal order and compares the values of the columns. Uses `std::string::equals` function for comparison. A row which has a value for the queried column is always before a column without the value. If the values of the column are equal, the rows are compared by the next column. If all of the specified columns are checked, the comparator returns `true` (that the `first` should be placed before `second`).
  
#### Conditions
Represents a set of conditions to a row of the table. They are used for filtering rows in queries (in the [shell](#Shell) they are used by the `WHERE` clause). Each condition is a `std::function<bool(const vector<string>&)>`, where the input is the row of a table and thereturned value is whether the condition holds on the row. The implementation is located in the `Conditions.hpp` and `Conditions.cpp` files.
##### Attributes
* `private vector<function<bool(const vector<string>&)>> conditions` : Is the set of the conditions for a row. The term 'set' is used as their order and multiple occurences do not matter for the result of the `Hold()` function.
##### Constructors
Has only the implicit constructor `Conditions()`. An empty set of contitions is always satisfied.
##### Functions
All functions are public.
###### Hold()
**Signature:** ` bool Hold(const vector<string>& row) const`

Tests all functions in the `conditions` vector. If any of them returns `false`, returns `false`. Otherwise returns `true`.
###### Functions for adding conditions
* `void AddEquals(int index, string str)` : Adds a condition evaluating the *string equaity* between the value of `index`-th column and `str`.
* `void AddIntLt(int index, string str)` : Adds a condition evaluating whether the `index`-th value can be converted to `int` and then performs a less than comparison between the int and the `str` converted to int. if the `str` cannot be converted to `int`, the condition always returns `false`.
* `void AddIntGt(int index, string str)` : Works like `AddIntGt()`, however evaluates whether the value is greater than the provided value.
* `void AddOther(function<bool(const vector<string>&)>&& condition)` : This allows to add any user specified condition with the corresponding interface.
---

### Helper functions
The `helper.hpp/cpp` contains a set of functions not bound to any class or file and used accross the whole namespace.

#### getQuotedString()
**Signature:** `string getQuotedString(istream& in, string::const_iterator &line_it, string& line, char quotechar, char endline)`
Given an input stream returns a token of a quoted string. The function does not check whether it is inside a quotes. Expects that the istream is 'on' the starting quotechar and automatically skips it. Uses *RFC4180*'s standard for treating quoted tokens:
> * Fields containing line breaks (CRLF), double quotes, and commas should be enclosed in double-quotes.
> * If double-quotes are used to enclose fields, then a double-quote appearing inside a field must be escaped by preceding it with another double quote.

#### 
**Signature:** `string trimmedString(const string& str)`

Returns a copy of the string without any leading and trailing whitespace characters. For the check uses `std::isspace()`.

#### formatRow()
**Signature:** `string formatRow(const vector<string>& cells, char delimiter, char quotechar, char endline)`
Returns a row of a csv table made from the `cells`. Each column is enclosed in the `quotechar`, followed by the `delimiter` and ended by the `endline`. Furthermore, all occurences of the `quotechar` inside any cell is escaped by doubling the original occurence (following the *RFC 4180* specifiation). No other characters are added into the output. For example:
```
input: ["a", "b", "c"], ',', '"', ';'
output: "a","b","c";
```
All generated files arre formatted by this function.

#### createJoinedSchema()
**Signature:** `vector<string> createJoinedSchema (const vector<string>& first, const vector<string>& second)`
Given 2 vectors of strings, returns a vector of strings containing a set union of the 2 vectors. Current implementation counts on the first vector containing no duplicities for effectivity reasons, as currently the function is used exclusively on table schemas which 'should' contain unique values by themselves.

#### index_of()
**Signature:** `int index_of(string str, const vector<string>& arr)`
Returns the position of the `str` in the `arr`. If the vector does not contain the string, returns -1 as it it always an invalid index. 


