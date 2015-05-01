opt:= -Wall -Werror -ansi -pedantic
compiler := g++
objdir := bin

all: bin rshell ls
bin:
	@test -d $(objdir) || mkdir $(objdir)
rshell: 
	@$(compiler) src/hw0.cpp $(opt) -o ./bin/rshell
ls: 
	@$(compiler) src/ls.cpp $(opt) -o ./bin/ls
cp:
    @$(compiler) cp.cpp $(opt) -o ./bin/cp
leak-check: all
	@valgrind --leak-check=full ./bin/ls
