opt:= -Wall -Werror -ansi -pedantic
compiler := g++
objdir := bin

all: bin ls

bin:
	@test -d $(objdir) || mkdir $(objdir)
ls: 
	@$(compiler) src/ls.cpp $(opt) -o ./bin/ls
	
leak-check: all
	@valgrind --leak-check=full ./bin/ls
	
