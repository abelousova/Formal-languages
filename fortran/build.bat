@echo off
flex -o"lex.yy.cpp" fortran.l
bison -d fortran.y
del fortran.tab.cpp
ren fortran.tab.c fortran.tab.cpp	
