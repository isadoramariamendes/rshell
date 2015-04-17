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
using namespace std;

vector<int> conn_order;
int words = 0;

/*
 *  POSTCONDITION: Prints the username and hostname, if possible.
 */
void userInfo() {
    char machine[100];
    struct passwd *password = getpwuid(getuid());
    int hostname = gethostname(machine, 100);
    
    if(password != NULL && hostname != -1) {
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
    if (commandLine == "exit") exit(0);
    char *commands = new char [commandLine.size() + 1];
    strcpy(commands, commandLine.c_str());
    
    return commands;
}

/*
 *  INPUT: char *cmd - holds a command.
 *  OUTPUT: True if it's a comment, or false if it's not.
 */
bool isComment(char *cmd){
    if(cmd[0] == '#') return true;
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
void tok_comment(char *cmd){
    cmd = strtok(cmd, "#");
}

/*
 *  INPUT: char *cmd - holds a command without comments already
 *  POSTCONDITION: Loops inside of the command line, char by char and
 *  pushes back some labels into a vector, whose value depends
 *  on the connector found.
 */
void read_order(char *cmd){
    for(int i = 0; cmd[i] != '\0'; ++i){
        if (cmd[i] == ';') {
            conn_order.push_back(1);
        }
        else if (cmd[i] == '&' && cmd[i + 1] == '&') {
            conn_order.push_back(2);
            ++i;
        }
        else if (cmd[i] == '|' && cmd[i + 1] == '|'){
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
int tok_conn(char **cmdlist, char *cmd){
    int argc = 0;
    cmdlist[argc] = strtok(cmd, ";&|");
    while(cmdlist[argc] != NULL){
        ++argc;
        cmdlist[argc] = strtok(NULL, ";&|");
    }
    return argc;
}

/*
 *  INPUT: char *cmd - holds a command.
 *  OUTPUT: True if it's exit, or false if it's not.
 */
bool isExit(char *c){
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
    char **temp = new char *[size + 1];
    
    while (curr != size) {
        int argc = 0;
        temp[argc] = strtok(cmdlist[curr], " ");
        while (temp[argc] != NULL) {
            if (isExit(temp[argc])) {
                exit(0);
            }
            ++argc;
            temp[argc] = strtok(NULL, " ");							//temp[argc] holds the "space-tokenized" commands
        }
        argc = 0;
        
        int pid = fork();
        if (pid == -1) {
            perror("fork");
        }
        else if (pid == 0) {
            int ret = execvp(temp[argc], temp);						//There is some black magic happening here
            if (ret == -1) {
                perror("execvp");
            }
            exit(1);
        }
        else {
            int ret;
            waitpid(pid,&ret,0);
            if (conn_order.size() > 0 && index < conn_order.size()) {
                if(ret != 0) {
                    if(conn_order.at(index) == 2) {
                        return;
                    }
                    ++index;
                }
                else {
                    if(conn_order.at(index) == 3) {
                        return;
                    }
                    ++index;
                }
            }
            ++curr;
        }
    }
}

int main()
{
    while(1) {
        
        userInfo();
        char *cmd = getCommands();
        
        if (!isComment(cmd)) {
            char **cmdlist = new char *[cmdSize(cmd) + 1];
            tok_comment(cmd);
            read_order(cmd);
            int list_size = tok_conn(cmdlist,cmd);
            tok_space(cmdlist, list_size);
        }
        conn_order.clear();
    }
    return 0;
}
