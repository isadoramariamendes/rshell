#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
using namespace std;

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON //in mac is MAP_ANON
#endif

vector<int> conn_order;
int words = 0;
static int *flag;
int sizeSpaceCmd = 0;
int sizeCommands = 0;
int sizeTmp = 0;
int sizeList = 0;

void freeList(char **StringList, unsigned long size) {
    if (StringList != NULL) {
        for (unsigned long i = 0 ; i < size ; i++) {
            if (StringList[i] != NULL) {
                delete[] StringList[i] ;
            }
        }
        delete[] StringList ;
    }
}

/*
 *  POSTCONDITION: Prints the username and hostname, if possible.
 */
void userInfo() {
    char machine[64];
    cout << endl;
    struct passwd *password = getpwuid(getuid());
    if (password == NULL) {
        perror("getpwuid");
    }
    int hostname = gethostname(machine, 64);
    if (hostname == -1) {
        perror("gethostname");
    }
    if (password != NULL && hostname != -1) {
        string username = password->pw_name;
        cout << username << "@" << machine << " ";
    }
    cout << "$ ";
}

/*
 *  POSTCONDITION: Prints the username and hostname, if possible.
 *  OUTPUT: a string of chars with all commands.
 */
char *getCommands() {
    string commandLine;
    getline(cin, commandLine);
    commandLine += '\0';
    if (commandLine == "exit") exit(0);
    sizeCommands = (int)commandLine.size();
    char *commands = new char [sizeCommands];
    strcpy(commands, commandLine.c_str());
    
    return commands;
}

/*
 *  INPUT: char *cmd - holds a command.
 *  OUTPUT: True if it's a comment, or false if it's not.
 */
bool isComment(char *cmd) {
    if (cmd[0] == '#') return true;
    return false;
}

/*
 *  INPUT: char *cmd - holds a command.
 *  OUTPUT: The size of the command.
 */
size_t cmdSize(char *cmd) {
    return strlen(cmd);
}

/*
 *  POSTCONDITION: Remove the comments from the command line.
 */
void tok_comment(char *cmd) {
    cmd = strtok(cmd, "#");
}

/*
 *  INPUT: char *cmd - holds a command without comments already
 *  POSTCONDITION: Loops inside of the command line, char by char and
 *  pushes back some labels into a vector, whose value depends
 *  on the connector found.
 */
void read_order(char *cmd) {
    for (int i = 0; cmd[i] != '\0'; ++i) {
        if (cmd[i] == ';') {
            conn_order.push_back(1);
        }
        else if (cmd[i] == '&' && cmd[i + 1] == '&') {
            conn_order.push_back(2);
            ++i;
        }
        else if (cmd[i] == '|' && cmd[i + 1] == '|') {
            conn_order.push_back(3);
            ++i;
        }
    }
}

/*
 *  INPUT:
 *  char **cmdlist - array of strings which will carry each individual command with its respective arguments;
 *  char *cmd - holds a command line that can have many other commands chained together using the "&& || ;" operators
 *  OUTPUT: The number of "argument-lines".
 *  POSTCONDITION: Everytime that the connectos "&& || ;" be found
 *  cmdline[x] will have that command and the number of commands will be increased by 1.
 */
int tok_conn(char **cmdlist, char *cmd) {
    int argc = 0;
    cmdlist[argc] = strtok(cmd, ";&|");
    while(cmdlist[argc] != NULL) {
        ++argc;
        cmdlist[argc] = strtok(NULL, ";&|");
    }
    return argc;
}

/*
 *  INPUT: char *cmd - holds a command.
 *  OUTPUT: True if it's exit, or false if it's not.
 */
bool isExit(char *c) {
    if (!strcmp(c, "exit")) {
        //cout << "BYE!" << endl;
        return true;
    }
    return false;
}

/*
 *  INPUT: **cmdlist - holds the lines with commands whose connectors were tokenized.
 *  int size - value is the number of arguments calculated to exist between all connectors.
 *  POSTCONDITION: Loops inside the function tokenizing an entire argument line's
 *  white space and organizes those tokens to be executed with 'execvp()'
 */
void tok_space (char **cmdlist, int size) {
    int curr = 0;
    unsigned index = 0;
    sizeTmp = size + 1;
    char **temp = new char *[sizeTmp];
    
    while (curr != size) {
        int argc = 0;
        temp[argc] = strtok(cmdlist[curr], " ");
        while (temp[argc] != NULL) {
            if (isExit(temp[argc])) {
                exit(0);
            }
            ++argc;
            temp[argc] = strtok(NULL, " ");
        }
        argc = 0;
        
        int pid = fork();
        if (pid == -1) {
            perror("fork");
        }
        else if (pid == 0) {
            int ret = execvp(temp[argc], temp);
            if (ret == -1) {
                perror("execvp");
            }
            exit(1);
        }
        else {
            int ret;
            waitpid(pid, &ret, 0);
            if (conn_order.size() > 0 && index < conn_order.size()) {
                if (ret != 0) {
                    if (conn_order.at(index) == 2) {
                        return;
                    }
                    ++index;
                }
                else {
                    if (conn_order.at(index) == 3) {
                        return;
                    }
                    ++index;
                }
            }
            ++curr;
        }
    }
    delete []temp;
}

char *addSpaces(char *cmd) {
    string line = string(cmd);
    
    for (unsigned int l = 0; l < line.length(); l++) {
        if (line[l]== '<' && line[l + 1] != '<') {
            line.insert(l + 1, " ");
            line.insert(l, " ");
            l++;
        }
        if (line[l]== '>' && line[l + 1]!= '>') {
            line.insert(l + 1, " ");
            if (!isdigit(line[l-1])) {
                line.insert(l, " ");
                l++;
            }
        }
        if (line[l]== '>' && line[l + 1] == '>') {
            line.insert(l + 2, " ");
            l = l + 2;
        }
        if (line[l]== '<' && line[l + 1] == '<' && line[l + 2] == '<') {
            line.insert(l + 3, " ");
            line.insert(l, " ");
            l = l + 3;
        }
        if (line[l]== '|' && line[l + 1] != '|') {
            line.insert(l + 1, " ");
            line.insert(l, " ");
            l++;
        }
    }
    
    sizeSpaceCmd = line.length() + 1;
    char *spaceCmd = new char[sizeSpaceCmd];
    strcpy(spaceCmd,line.c_str());
    
    return spaceCmd;
}

void execute (char *cmd, char **cmdlist) {
    read_order(cmd);
    int list_size = tok_conn(cmdlist, cmd);
    tok_space(cmdlist, list_size);
}

//function recieves commands and arguments before '>' and file to be output
void out_redirect(char *cpystr[], char *file_out, bool symbol, int fd_number) {
    for (int i =0; i<3; i++) {
        
    }
    int fdo;
    //open file descriptor as the argument after '>'
    if (symbol) {
        fdo = open(file_out, O_RDWR|O_CREAT|O_APPEND, 0666);
        if (fdo == -1) {
            perror("open");
            exit(1);
        }
    }
    else{
        fdo = open(file_out, O_RDWR|O_CREAT, 0666);
        if (fdo == -1) {
            perror("open");
            exit(1);
        }
    }
    if (fd_number!=0) {
        if (dup2(fdo,fd_number) == -1) {
            perror("dup");
            exit(1);
        }
    }
    
    if (execvp(cpystr[0], cpystr) == -1)
        perror("execvp");
}


//function recieves commands before '<' and file to be input
void in_redirect(char * cpystr[], char * file_in) {
    int fdi;
    
    fdi = open(file_in, O_RDONLY);
    if (fdi == -1) {
        perror("open");
        exit(1);
    }
    if (dup2(fdi,0) == -1) {
        perror("dup");
        exit(1);
    }
    
    if (execvp(cpystr[0], cpystr) == -1)
        perror("execvp 'in' failed");
}

void in_redirect2(char * cpystr[], int pos, char * str[], int size) {
    
    if (memcmp(str[pos+1], "\"", 1) == 0) {
        int fd[3];
        if (pipe(fd)==-1) {
            perror("pipe");
            exit(1);
        }
        
        unsigned long len = strlen(str[size-1]) -1;
        //removing quote marks
        memmove(&str[pos+1][0], &str[pos+1][0 + 1], strlen(str[pos+1]));
        memmove(&str[size-1][len], &str[size-1][len+1], strlen(str[size-1]) - len);
        
        //add all the string after '<<<'
        for (int i=pos+1; i<size; i++) {
            if (-1 == write(fd[1],str[i],strlen(str[i]))) {
                perror("write");
            }
            if (-1 == write(fd[1]," ",1)) {
                perror("write");
            }
        }
        if (-1 == write(fd[1],"\n",1)) {
            perror("write");
        }
        if (dup2(fd[0],0) == -1) {
            perror("dup");
            exit(1);
        }
        
        if (close(fd[1]) == -1) {
            perror("close");
        }
        
        if (execvp(cpystr[0], cpystr) == -1)
            perror("execvp 'in' failed");
    }
    else if (size == 3) cout << str[pos+1] << endl;
    else cout << "ERROR: no such file or directory" << endl;
}

//working
int search_pipe(char *str[], int size) {
    for (int i = 0; i < size; i++) {
        if (memcmp(str[i], "|\0", 2) == 0) {
            //cout << str[i] << " Pipe found in: " << i << endl;
            return i;
        }
    }
    return -1;
}

int search_lessthanSign(char *str[], int size) {
    for (int i=0; i<size; i++) {
        if (memcmp(str[i], "<\0", 2) == 0) {
            return i;
        }
    }
    return -1;
}

void redirect(char * str[], int size) {
    int i, j;
    char *cpystr[512];
    
    for (i = 0;i < size; i++) {
        if (*flag != -1)
            *flag = 0;
        int aux = 0;
        for (j = i; j < size; j++) {
            if (memcmp(str[j], "<\0", 2) == 0) {
                *flag = 1;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if ((memcmp(str[j], ">\0", 2) == 0) || (memcmp(str[j], "1>\0", 3) == 0)) {
                *flag = 2;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if ((memcmp(str[j], ">>\0", 3) == 0) || (memcmp(str[j], "1>>\0", 4) == 0)) {
                *flag = 3;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "|\0", 2) == 0) {
                i = j;
                break;
            }
            if (memcmp(str[j], "<<<\0", 4) == 0) {
                *flag = 4;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "2>\0", 3) == 0) {
                *flag = 5;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "2>>\0", 4) == 0) {
                *flag = 6;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "0>", 2) == 0) {
                *flag = 7;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            cpystr[aux] = str[j];
            aux++;
            i = j;
        }
        
        cpystr[aux] = (char*)'\0';
        aux++;
        
        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {
            if (*flag == 1)in_redirect(cpystr, str[j+1]);
            else if (*flag == 2)out_redirect(cpystr, str[j+1], false, 1);
            else if (*flag == 3)out_redirect(cpystr, str[j+1], true, 1);
            else if (*flag == -1) {
                if (execvp(cpystr[0], cpystr) == -1) {
                    perror("execvp");
                }
            }
            else if (*flag == 4) in_redirect2(cpystr, i, str, size);
            else if (*flag == 5) out_redirect(cpystr, str[j+1], false, 2);
            else if (*flag == 6) out_redirect(cpystr, str[j+1], true, 2);
            else if (*flag == 7) out_redirect(cpystr, str[j+1], true, 0);
            exit(1);
        }
        else if (pid > 0) {
            int status;
            wait(&status);
            if (-1 == status) {
                perror("wait");
            }
        }
        
        // clear cpystr in order to execute + commands
        memset(&cpystr[0], 0, sizeof(cpystr));
    }
}

void piping(int index, int size, char *str[]) {
    
    char *cpystr[256];
    char *restof_str[256];
    
    int end = 0;
    int restof_size = 0;
    //add whatever is before | to string
    for (int i = 0; i < index; i++) {
        cpystr[i] = str[i];
        end = i;
    }
    cpystr[end + 1] = (char *)'\0';
    
    //add what ever is after the first | to string
    for (int i = index + 1; i < size; i++) {
        restof_str[restof_size] = str[i];
        restof_size++;
    }
    restof_str[restof_size + 1] = (char*)'\0';
    
    int fd[2];
    if (pipe(fd) == -1) {
        perror ("pipe");
        exit(1);
    }
    
    
    int pid = fork();
    
    if (pid == -1) {
        perror("fork");
    }
    else if (pid == 0) {
        // child doesn't read
        if (close(fd[0]) == -1) {
            perror("close");
        }
        // redirect stdout
        if (dup2(fd[1],1) == -1) {
            perror("dup");
        }
        
        int check = search_lessthanSign(cpystr, end);
        if (check == -1) {
            if (-1 == execvp(cpystr[0], cpystr)) {
                perror("execvp");
            }
        }
        else  {
            //if line has pipes but start with redirection <
            redirect(cpystr, end);
        }
        exit(1);
        
    }
    else {
        
        int c_in = dup(0);
        if (c_in == -1) {
            perror("Dup failed.");
        }
        // parent doesn't write
        if (close(fd[1]) == -1) {
            perror("Close failed.");
        }
        // redirect stdin
        if (dup2(fd[0],0) == -1) {
            perror("Dup failed.");
        }
        if (wait(0) == -1) {
            perror("Wait failed");
        }
        //look for + pipe
        int chain = search_pipe(restof_str, restof_size);
        if (chain != -1) {
            //execute next pipe command
            piping(chain, restof_size, restof_str);
        }
        else {
            //if no more pipes
            *flag = 14;
            redirect(restof_str, restof_size);
        }
        
        if (dup2(c_in,0) == -1) {
            perror("Dup failed.");
        }
        cout << flush;
        cin.clear();
    }
}

//works
int checkProcedure(char *str[], int size) {
    for (int i = 0; i < size; i++) {
        if (memcmp(str[i], "|\0", 2) == 0) {
            //cout << "Procedure to | " << endl;
            return 1;
        }
        if (memcmp(str[i], "<", 1) == 0) {
            //cout << "Procedure to < " << endl;
            return 1;
        }
        if (memcmp(str[i], ">", 1) == 0) {
            //cout << "Procedure to > " << endl;
            return 1;
        }
        if (memcmp(str[i], "2>", 2) == 0) {
            //cout << "Procedure to 2> " << endl;
            return 1;
        }
        if (memcmp(str[i], "1>", 2) == 0) {
            //cout << "Procedure to 1> " << endl;
            return 1;
        }
        if (memcmp(str[i], "0>", 2) == 0) {
            //cout << "Procedure to 0> " << endl;
            return 1;
        }
    }
    return 0;
}

//working
int search_pipe(char *str[], int size) {
    for (int i = 0; i < size; i++) {
        if (memcmp(str[i], "|\0", 2) == 0) {
            //cout << str[i] << " Pipe found in: " << i << endl;
            return i;
        }
    }
    return -1;
}

int search_lessthanSign(char *str[], int size) {
    for (int i=0; i<size; i++) {
        if (memcmp(str[i], "<\0", 2) == 0) {
            return i;
        }
    }
    return -1;
}

void redirect(char * str[], int size) {
    int i, j;
    char *cpystr[512];
    
    for (i = 0;i < size; i++) {
        if (*flag != -1)
            *flag = 0;
        int aux = 0;
        for (j = i; j < size; j++) {
            if (memcmp(str[j], "<\0", 2) == 0) {
                *flag = 1;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if ((memcmp(str[j], ">\0", 2) == 0) || (memcmp(str[j], "1>\0", 3) == 0)) {
                *flag = 2;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if ((memcmp(str[j], ">>\0", 3) == 0) || (memcmp(str[j], "1>>\0", 4) == 0)) {
                *flag = 3;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "|\0", 2) == 0) {
                i = j;
                break;
            }
            if (memcmp(str[j], "<<<\0", 4) == 0) {
                *flag = 4;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "2>\0", 3) == 0) {
                *flag = 5;
                cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "2>>\0", 4) == 0) {
                *flag = 6;
                cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            if (memcmp(str[j], "0>", 2) == 0) {
                *flag = 7;
                //cout << "Flag :" << *flag << endl;
                i = j;
                break;
            }
            cpystr[aux] = str[j];
            aux++;
            i = j;
        }
        
        cpystr[aux] = (char*)'\0';
        aux++;
        
        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {
            if (*flag == 1)in_redirect(cpystr, str[j+1]);
            else if (*flag == 2)out_redirect(cpystr, str[j+1], false, 1);
            else if (*flag == 3)out_redirect(cpystr, str[j+1], true, 1);
            else if (*flag == -1) {
                if (execvp(cpystr[0], cpystr) == -1) {
                    perror("execvp");
                }
            }
            else if (*flag == 4) in_redirect2(cpystr, i, str, size);
            else if (*flag == 5) out_redirect(cpystr, str[j+1], false, 2);
            else if (*flag == 6) out_redirect(cpystr, str[j+1], true, 2);
            else if (*flag == 7) out_redirect(cpystr, str[j+1], true, 0);
            exit(1);
        }
        else if (pid > 0) {
            int status;
            wait(&status);
            if (-1 == status) {
                perror("wait");
            }
        }
        
        // clear cpystr in order to execute + commands
        memset(&cpystr[0], 0, sizeof(cpystr));
    }
}

void piping(int index, int size, char *str[]) {
    
    char *cpystr[256];
    char *restof_str[256];
    
    int end = 0;
    int restof_size = 0;
    //add whatever is before | to string
    for (int i = 0; i < index; i++) {
        cpystr[i] = str[i];
        end = i;
    }
    cpystr[end + 1] = (char *)'\0';
    
    //add what ever is after the first | to string
    for (int i = index + 1; i < size; i++) {
        restof_str[restof_size] = str[i];
        restof_size++;
    }
    restof_str[restof_size + 1] = (char*)'\0';
    
    int fd[2];
    if (pipe(fd) == -1) {
        perror ("pipe");
        exit(1);
    }
    
    
    int pid = fork();
    
    if (pid == -1) {
        perror("fork");
    }
    else if (pid == 0) {
        // child doesn't read
        if (close(fd[0]) == -1) {
            perror("close");
        }
        // redirect stdout
        if (dup2(fd[1],1) == -1) {
            perror("dup");
        }
        
        int check = search_lessthanSign(cpystr, end);
        if (check == -1) {
            if (-1 == execvp(cpystr[0], cpystr)) {
                perror("execvp");
            }
        }
        else  {
            //if line has pipes but start with redirection <
            redirect(cpystr, end);
        }
        exit(1);
        
    }
    else {
        
        int c_in = dup(0);
        if (c_in == -1) {
            perror("Dup failed.");
        }
        // parent doesn't write
        if (close(fd[1]) == -1) {
            perror("Close failed.");
        }
        // redirect stdin
        if (dup2(fd[0],0) == -1) {
            perror("Dup failed.");
        }
        if (wait(0) == -1) {
            perror("Wait failed");
        }
        //look for + pipe
        int chain = search_pipe(restof_str, restof_size);
        if (chain != -1) {
            //execute next pipe command
            piping(chain, restof_size, restof_str);
        }
        else {
            //if no more pipes
            *flag = 14;
            redirect(restof_str, restof_size);
        }
        
        if (dup2(c_in,0) == -1) {
            perror("Dup failed.");
        }
        cout << flush;
        cin.clear();
    }
}

//works
int checkProcedure(char *str[], int size) {
    for (int i = 0; i < size; i++) {
        if (memcmp(str[i], "|\0", 2) == 0) {
            //cout << "Procedure to | " << endl;
            return 1;
        }
        if (memcmp(str[i], "<", 1) == 0) {
            //cout << "Procedure to < " << endl;
            return 1;
        }
        if (memcmp(str[i], ">", 1) == 0) {
            //cout << "Procedure to > " << endl;
            return 1;
        }
        if (memcmp(str[i], "2>", 2) == 0) {
            //cout << "Procedure to 2> " << endl;
            return 1;
        }
        if (memcmp(str[i], "1>", 2) == 0) {
            //cout << "Procedure to 1> " << endl;
            return 1;
        }
        if (memcmp(str[i], "0>", 2) == 0) {
            //cout << "Procedure to 0> " << endl;
            return 1;
        }
    }
    return 0;

int main()
{
    char **cmdlist = 0;
    char *cmdSpaced = 0;
    char *cmd = 0;
    while(1) {
        
        userInfo();
        cmd = getCommands();
        
        if (!isComment(cmd)) {
            tok_comment(cmd);
            if (memcmp(cmd, "exit", 4) == 0) exit(0);
            //cout << cmd << " Cmd" << endl;
            
            cmdSpaced = addSpaces(cmd); //copy with spaces when occur appearence of | or < >
            //cout << cmdSpaced << " CmdSpaced" << endl;
            int index = 0;
            cmdlist = new char *[strlen(cmdSpaced)]; //this allocates much more memory than needed, but works without the risk of segfault and is easy
            char *pch = strtok(cmdSpaced, " ");
            while (pch != NULL) {
                cmdlist[index] = pch;
                //cout << str[index] << " index: " << index << endl;
                pch = strtok(NULL, " ");
                index++;
            }
            cmdlist[index] = NULL;
            
            //create a shared memory to be accessed from child and process
            flag = (int*)mmap(NULL, sizeof *flag, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            
            int pos = search_pipe(cmdlist, index);
            if (pos != -1) {
                //cout << "POS == -1" << endl;
                piping(pos, index, cmdlist);
            }
            else if (checkProcedure(cmdlist, index)){
                //cout << "CHECKPROCEDURE" << endl;
                redirect(cmdlist, index);
            }
            else {
                execute(cmd, cmdlist);
                //cout << " CMD NORMAL" << endl;
            }
            delete [] cmd;
            delete [] cmdSpaced;
        }
        conn_order.clear();
        //free
        
    }
    return 0;
}
