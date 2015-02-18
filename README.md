# Calcula-Language-Interpreter
Programming Languages Homework Project2 - C

Calcula is an arithmetical imaginary language. Our program loads and interprets (execute) Calcula scripts. Our program also includes lexical analysis and parser for Calcula language.

----

-What is Calcula Language?

A Calcula script is composed of just a sequence of lines. There is no block structure. The whole code is also stored in one file that has a .cal extension. Files with this extension are regarded as executable.
Code is composed of two kinds of lines: 

Assignment: |identifier| := |expression|;
    
Output: Output |output_list|;

|expression| covers the arithmetic expressions in which only integers are involved. All variables in the language belong to the integer type. They are considered to be declared at the time they were used for the first time.    

**Calcula is not case sensitive. 

----

Example of Calcula program:

  A:=-2; Bee:=5;
  
  C:=bee/A;
  
  Output BEE “ divided by ” a “ gives:” C;
