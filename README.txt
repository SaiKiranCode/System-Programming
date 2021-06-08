sic/xe program supporting program relocation and symbol defining

compiler is g++ compiler
ways to :
comiple :  g++ program.txt
run :   ./a.out input.txt
(here the input program is passed as cammand line argument)

files:
input.txt: source program
program.cpp: main sic/xe assembler program
intermediate.txt : intermediate file generated after pass1 of assembler
Symtab.txt : symbol table with symbols and there addresses
optable.txt : operand table
object_code.txt:final objective program
pass2_output : pass2 output with objectcode assigned to each expression


(you can check the program by using program.cpp with files input.txt,optable.txt ,README.txt)
(submitted by 17CSE1007&&17CSE1019)
