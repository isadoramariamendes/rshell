# rshell
# writing a basic command shell

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
Running bin/rshell inside of the rshell works.

# Installation
To execute the program just follow the steps:
1 - Download the project

2 - Open the terminal

3- Go inside of the directory rshell by typing:

`cd <path of the directory here>/rshell`

4 - Once you are in the right directory, just type 

`$ make` 

After that, if you want to run the software, type:

`$ bin/rshell

to run the software or;

`$ make leak-check`

to check some memory leaks;

# Dependencies

By now this software do not use any external library.

#Bugs/Limitations/Issues

Sometimes occurs unexpected erros which seems to be something releated with the mallocs that the new operator makes. 
However, it is not always, so I am trying to debug everything to do not happen this anymore.
When trying to do the command: `echo "Hi!" > README` The following error appears SOMETIMES:
`*** glibc detected *** bin/rshell: malloc(): memory corruption: 0x0000000000af6270 ***`
and sometimes just echo `"Hi!" > README` as expected since I did not treated `>` and `<` as connectors.
I.e., I am having an undefined behavior.

