all: 
	g++ hw0.cpp -o hw0 -Wall -Werror -ansi -pedantic

run: all
	./hw0

leak-check: all
	@valgrind --leak-check=full ./hw0
