opt:= -Wall -Werror -ansi -pedantic
compiler := g++
objdir := bin

all: bin rshell

bin:
	test -d $(objdir) || mkdir $(objdir)
rshell: 
	$(compiler) hw0.cpp $(opt) -o ./bin/rshell
