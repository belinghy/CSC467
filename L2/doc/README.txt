CSC467 Phase 2 Report

Design/Approach
    We used the code provided by the starter kit as a general framework and added the grammar to parser.y as specified in lab specification. We then test if the parser works as intended using the test cases specified below. If any    unintended behaviour was produced by the parser, we would then go back to change our code and repeat the testing process again. When resolving a problem, we consulted first the lecture notes to find material pertinent to the problem. If we are still unable to resolve the problem, we would then consult the bison manual provided by GNU.

Implementation
    For this assignment, we chose to use Bison to implement a LR(1) grammar instead of writing a parser from scratch to implement a LL(k) grammar. In order to use Bison to parse the grammar, we fixed the ambiguities of the grammar in the specification by modifying existing grammar rules and adding additional precedence rules. We also implemented the -Tp flag so that setting the flag will cause the parser to print out the grammar rule that was used with every reduce. 

Testing/Verification
    We tested the code with several test files contained in the cases/ directory:
    Expected to pass:
        test_arith_ops.txt - test that precedence and associativity for the operators are followed
        test_dangling_else.txt - test that various cases involving if-else statements are parsed correctly
        test_functions.txt - test that constructors, loops and functions are accepted
        test_simple_code.txt - simple GLSL code with various rules invoked

    Expected to fail:
        test_no_scope_fail.txt - should throw an error due to missing enclosing scope
        test_missing_semicolon_fail.txt - should throw an error due a missing semicolon
        test_functions_fail.txt - should throw an error due to function not being assigned to some variable; standalone function calls are not supported

    With each test, we ran the parser with the test file and the -Tp and -Tn flag set. The correctness of each test is checked manually by observing the output of the compiler.

Challenges/Issues
    - The declaration: epsilon rule results in warning “rule useless due to conflict” because there is already a declarations:epsilon rule. We removed the first rule.
    - The shift reduce conflict when the next token is ‘=’ is resolved by giving ‘=’ a lower precedence than the unary operator.
    - The dangling else ambiguity is resolved by specifying in the grammar that each ELSE statement is paired with the most recent IF (unless a scope is used to clarify the pairings) by using a combination of grammar rules and precedence.
    - The binary_op grammar rules are removed and added to the expression grammar rules in order to allow the precedence rules of the operators to take effect and avoid shift-reduce conflicts.

Breakdown of work
    We chose eXtreme Programming as our development methodology where both team members performed paired programming and have collective code ownership. If a bug occurred during the testing phase, both team members are responsible to fix the bug collectively.
