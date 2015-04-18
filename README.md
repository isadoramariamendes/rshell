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

i.e, the part that goes after the connector || won't be executed because the parte before || is true. However, in case of executing the same line again, the result would be:


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

that's because mkdir test failed. So the parte of the command that comes after the connector || must be executed.`
In the tests directory can be found a file called exec.script that contains all of the test cases I tried.

# Installation
To execute the program just follow the steps:
1 - Download the project

2 - Open the terminal

3- Go inside of the directory rshell by typing:

`cd <path of the directory here>/rshell`

4 - Once you are in the right directory, just type 

`$ make` 

After that, if you want to run the software, type:

`$ bin/rshell`

to run the software or;

`$ make leak-check`

to check some memory leaks using valgrind;

# Dependencies

By now this software do not use any external library.

#Bugs/Limitations/Issues

Sometimes occurs unexpected erros which seems to be something releated with the mallocs that the new operator makes. 
However, it is really rare to happen. I am trying to debug everything to do not happen this anymore using valgrind.
When trying to do the command: `echo "Hi!" > README` The following error appears SOMETIMES:

`*** glibc detected *** bin/rshell: malloc(): memory corruption: 0x0000000000af6270 ***`

but sometimes just echo `"Hi!" > README` as expected since I did not treated `>` and `<` as connectors.
i.e., I am having an undefined behavior.

