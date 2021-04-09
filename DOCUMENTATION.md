# Technical documentation
This document serves to depict the internal workings of the BigCSV command line utility, as well as the whole BigCSV namespace. 

## Structure
All of the code is located in `BigCSV` namespace. Each class is declared in a single `.hpp` file and defined in a `.cpp` file of the same name. Then there are `main.cpp` file serving to launch an instance of `Shell` and `helper.hpp` and `helper.cpp` containing functions not associated to any class. The program can be logically divided into 2 independent parts: the Database and the Shell. The functions in the [`Helper class`](#Helper) are used by both.

### Shell
The shell proivides an interface of a command line utility to use the functionality of the BigCSV namespace. It is responsible for accepting and parsing user input and executing appropriate functions of the BigCSV namespace. In the following sections, its inner structure is explained. For user manual, please refer to the *User Manual*.

#### Attributes
* `private istream& in_stream` : The input stream from which the commands are accepted
* `private ostream& err_stream` : The output stream for error messages 
* `private map<std::string, csvTable> tables` : A map containing the user defined tables by their names (as specified by the user)
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
Following 3 functions are used to parse the `SET` clauses of commands by maintaining and updating a map (attribute name -> attribute value) handled accross these functions.

##### set_map_attribute()
**Signature:** `bool set_map_attribute(const string &pair, map<string, string> &attributes) const`

Given an attribute pair in the form `attribute=value`, sets the value of the attribute in the `attribustes` map accordingly. Returns a boolean signifying whether the action was successful. The value pair is checked if it is in the prescribed form. Then the name of the attribute is checked if it is in the provided `attributes` map. If some of the checks fail, the user is informed via the `err_stream` and `false`  is returned. All values are passed as strings. A value may be enclosed in `"` (double-quotes), then the `getNextWord()` function is used to process it into a token.

##### modify_attribute_map()
**Signature:** `bool modify_attribute_map(int& index, map<string, string>& atts) const`

Given an index into the `command` and a map of attributes to modify, parses the `SET` clause of command (using `set_map_attribute()`) and returns a boolean signalizing whether any problenms arose during the process. In such case, the reason of the problem is sent onto the `err_stream`. If a value for an attribute is specified in multiple `attribute=value` pair, the last pair is used.

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
##### Constructors
##### Functions

#### csvFile
##### Attributes
##### Constructors
##### Functions

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
##### Attributes
##### Constructors
##### Functions

#### RowUpdate
##### Attributes
##### Constructors
##### Functions

#### RowComparator
##### Constructors
##### Attributes
##### Functions

#### Conditions
##### Attributes
##### Constructors
##### Functions

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
Returns a row of a csv table made from the `cells`. Each column is enclosed in the `quotechar`, followed by the `delimiter` and ended by the `endline`. No other characters are added into the output. For example:
```
input: ["a", "b", "c"], ',', '"', ';'
output: "a","b","c";
```
All generated files arre formatted by this function.

#### split()
**Signature:** `vector<string> split(const string& in, const char delim)`
An implementation of the standard split operation on a string. Returns a vector of tokens created from the `in` by splitting it by the `delim` delimiting character.

#### createJoinedSchema()
**Signature:** `vector<string> createJoinedSchema (const vector<string>& first, const vector<string>& second)`
Given 2 vectors of strings, returns a vector of strings containing a set union of the 2 vectors. Current implementation counts on the first vector containing no duplicities for effectivity reasons, as currently the function is used exclusively on table schemas which 'should' contain unique values by themselves.

#### index_of()
**Signature:** `int index_of(string str, const vector<string>& arr)`
Returns the position of the `str` in the `arr`. If the vector does not contain the string, returns -1 as it it always an invalid index. 


