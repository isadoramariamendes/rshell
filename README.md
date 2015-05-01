#ls
This program is a C++ program that implements a subset of the GNU ls command. It is expected to support: the -a, -l and -R optional flags; and the ability to pass in zero, one, or many files. 

To implement ls, I used the opendir, closedir, readdir and stat syscalls, which were all checked using `perror`. 

# Installation
To execute the program just follow the steps:
1 - Clone the project

2 - Open the terminal

3- Go inside of the directory rshell by typing:

`cd <path of the directory here>/rshell`

4 - Type:

`git checkout hw1`

5 - Once you are in the right directory, type:

`$ make` 

6 - If you want to run the software, type:

`$ bin/ls -<optional labels here>`

to run the software or;

`$ make leak-check`

to check some memory leaks using valgrind;


# Dependencies

By now this software do not use any external library.

#Bugs/Limitations/Issues

The -R label was not implemented, so any use of the label _R will be executed as it wasn't there.

______________________________________________________________________________________

# rshell

This shell will perform the following steps:

1. Print the logged user and host if possible. So the command prompt `$` appears.

2. Read in a command on one line.
Commands will have the form:
```
cmd         = executable [ argumentList ] [ connector cmd ]
connector   = || or && or ;
```
where `executable` is an executable program in the `PATH` and `argumentList` is a list of zero or more arguments separated by spaces.
The connector is an optional way you can run multiple commands at once.
If a command is followed by `;`, then the next command is always executed;
if a command is followed by `&&`, then the next command is executed only if the first one succeeds;
if a command is followed by `||`, then the next command is executed only if the first one fails.
For example:
```
$ ls -a
$ echo hello
$ mkdir test
```
is equivalent to:
```
$ ls -a; echo hello; mkdir test
```
There is no limit to the number of commands that can be chained together using these operators,
and this program is able to handle any combination of operators.
Example that this program can handle:
```
$ ls -a; echo hello && mkdir test || echo world; git status
```
In case of commands associated by connectors this program will execute from left to right.
Let's suppose that the command above is executed one time. In that case, the result would be:
```
ls -a; echo hello && mkdir test || echo world; git status
.  ..  bin  .git  hw0.cpp  LICENSE  main.cpp  Makefile	README.md  test
hello
```

i.e, the part that goes after the connector `||` won't be executed because the parte before `||` is true. However, in case of executing the same line again, the result would be:


```
.  ..  bin  .git  hw0.cpp  LICENSE  main.cpp  Makefile	README.md  test
hello
mkdir: cannot create directory `test': File exists
world
# On branch master
# Untracked files:
#   (use "git add <file>..." to include in what will be committed)
#
#	<list of untracked files here>
#	nothing added to commit but untracked files present (use "git add" to track)
```

that's because `mkdir test` failed, since the directory was already created. So the part of the command that comes after the connector `||` must be executed.`
In the tests directory can be found a file called exec.script that contains all of the test cases I tried. Summaryzing, it can execute others programs, the own rshell program, handle connectors aforementioned, `CTRL-C` and etc.

# Installation
To execute the program just follow the steps:
1 - Clone the project

2 - Open the terminal

3- Go inside of the directory rshell by typing:

`cd <path of the directory here>/rshell`

4 - Type:

`git checkout hw0`

5 - Once you are in the right directory, type:

`$ make` 

6 - If you want to run the software, type:

`$ bin/rshell`

to run the software or;

`$ make leak-check`

to check some memory leaks using valgrind;


# Dependencies

By now this software do not use any external library.

#Bugs/Limitations/Issues

Any command thar depends on `>` and `<` as connectors do not work properly. Because of it sometimes it has undefined behavior when executing `"Hi!" > README`. Sometimes it just echo `"Hi!" > README` as expected, but sometimes I have other errors.
