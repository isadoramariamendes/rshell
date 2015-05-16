opt:=  -Wall -Werror -ansi -pedantic
compiler := g++
objdir := bin

all: bin rshell ls cp rm mv
bin:
	@test -d $(objdir) || mkdir $(objdir)
rshell: 
	@$(compiler) src/hw0.cpp $(opt) -o ./bin/rshell
ls: 
	@$(compiler) src/ls.cpp $(opt) -o ./bin/ls
cp:
	 @$(compiler) src/cp.cpp $(opt) -o ./bin/cp
rm: 
	@$(compiler) src/rm.cpp $(opt) -o ./bin/rm
mv:
	@$(compiler) src/mv.cpp $(opt) -o ./bin/mv
leak-check: all
	@valgrind --leak-check=full --show-reachable=yes  ./bin/rshell 

