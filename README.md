# CVSReader
Application for reading Comma-Separated-Values files. Accept files which cells consist either from positive integeres, either from formula like =ARG1 OP ARG2,
Where ARG1 and ARG2 are adresses of cells in Name_of_column Name_of_row and OP - is operation like "+","-","*","/".
Example
,A,B,Cell
1,1,0,1
2,2,=A1+Cell30,0
30,0,=B1+A1,5
After executing program will print in console:
,A,B,Cell
1,1,0,1
2,2,6,0,
30,0,1,5
