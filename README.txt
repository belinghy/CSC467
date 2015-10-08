CSC467 Phase 1 Report

Usage:
We included a lab.env file for easy testing:
-> source lab.env 
-> build; run
This will output the tests entered in test_cases.txt into token_output.txt.

Alternatively, ./compiler467 -Tn <input_file> -R <output_file> will dump the tokens scanned from the input_file into the output file.

Approach: 
When specifying regular expressions, we group tokens into types such as operators and keywords for conveniences, but we scan each operator or keyword into separate tokens. The only tokens grouped into categories are the different types of literals and the identifiers. Each built-in function is also recognized as its own type of token.
Assumptions / Limitations:
We limit integers to the range +2097151 to -2097152 (i.e. signed 22-bit values)
We limit variable names to 32 characters
We support only decimal numbers (not binary, octo-, or hexa- decimal).
We do not support scientific notation.
We do not have a limit for the size of floating point numbers and the limit of the precision will be determined by the behaviour of the atof function of C.
At this stage, we will treat built-in variables as regular identifiers - they will be accounted for in future labs during the semantics check and code generator stages.

Issues encountered:
To deal with undefined behaviour of atoi() function when integers exceed the maximum size limit of C, we assume integers are stored as 22 bits as per the language specification and write our own strToInt function that checks the size of the integer before we pass it to atoi. If the size is too large and will cause an overflow, the scanner will exit with error message “Can’t convert string to int”.

Currently, our integers and decimals are scanned including the sign (i.e. “+3” or “-0.2”). This is ambiguous when we have an expression involving addition or subtraction (i.e. “4+0.5”) where the operators will be considered part of the numbers rather than as a separate token. (i.e. “4” and “+0.5” rather than “4”, “+”, “0.5”). We consider this issue part of the grammar and will fix this in the parser phase of this project.

Tests:
We use test_cases.txt to specify specific test strings and test_input.txt to test with some real GLSL code. Since GLSL code is not quite mini-GLSL, some functionality is unaccounted for.

Responsibilities:
We write our program using mainly the programming collaboration technique of “paired programming”, where one group member types while the other member reviews, and the two roles are frequently reversed. We also committed our code to a private GitHub repository, and sometimes separately update the files after a discussion to determine the changes to be made. Each group member is responsible for understanding all of the code submitted and for documenting and committing the code that they add individually.

