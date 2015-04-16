#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
using namespace std;

vector<int> conn_order;													//vector to track order and type of connectors
int words = 0;

bool isExit(char *cmd){
    if (strcmp("exit", cmd)) return false;
    return true;
}

bool isEmpty(char *cmd){
    if(strlen(cmd) == 0) return true;
    return false;
}

bool isComment(char *cmd){
    if(cmd[0] == '#') return true;
    return false;
}

size_t cmdSize (char *cmd) {
    return strlen(cmd);
}

void disp_v(vector<int> &v){
    for(unsigned i=0; i<v.size(); ++i){
        cout << v.at(i);
    }
    cout << '\n';
}

void count_words(char *cmds){
    for(unsigned i=0; i<conn_order.size(); ++i){
        if(conn_order.at(i) == -1) ++words;
    }
}

char tok_comment(char *cmd){
    cmd = strtok(cmd, "#");
}

int tok_conn(char **cmdlist, char *cmd){
    int argc = 0;
    cmdlist[argc] = strtok(cmd, ";&|");
    while(cmdlist[argc] != NULL){
        ++argc;
        cmdlist[argc] = strtok(NULL, ";&|");
    }
    return argc;
}

void tok_space(char **cmdlist, int size){
    int curr = 0;
    unsigned index = 0;
    char **temp;
    temp = new char *[words + 1];
    while(curr < size){
        int argc = 0;
        temp[argc] = strtok(cmdlist[curr], " ");
        while(temp[argc] != NULL){
            if(isExit(temp[argc])){
                exit(0);
            }
            ++argc;
            temp[argc] = strtok(NULL, " ");							//temp[argc] holds the "space-tokenized" commands
        }
        argc = 0;
        
        int pid = fork();
        if(pid == -1){
            perror("fork");
        }
        else if(pid == 0){
            int ret = execvp(temp[argc], temp);						//There is some black magic happening here
            if(ret == -1){
                perror("execvp");
            }
            exit(1);
        }
        else{
            int ret;
            waitpid(pid,&ret,0);
            if(conn_order.size() > 0 && index < conn_order.size()){
                if(ret != 0){
                    if(conn_order.at(index) == 2){
                        return;
                    }
                    ++index;
                }
                else{
                    if(conn_order.at(index) == 3){
                        return;
                    }
                    ++index;
                }
            }
            ++curr;
        }
    }
}

/*
 *  OUTPUT: char *cmd - holds a command line.
 *   POSTCONDITION: Allocates memory to each char read from the command line.
 *    Puts '\0' at the end to mark end of commanding.
 *  
 */
char *readCmd() {
    
    char *cmd = (char*) malloc(sizeof(char));
    char c;
    
    int i = 0;
    int j = 1;
    
    printf("$ ");
    while(c != '\n')
    {
        c = getc(stdin);
        cmd = (char*)realloc(cmd,j*sizeof(char));
        cmd[i] = c;
        i++;
        j++;
    }
    cmd[i]='\0';
    
    return cmd;
}

/*
 *  INPUT: char *cmd - holds a command without comments already
 *   POSTCONDITION: Loops inside of the command line, char by char and
 *    pushes back some labels into a vector, whose value depends
 *     on the connector found.
*/
void readOrder(char *cmd){
    for (int i = 0; cmd[i] != '\0'; ++i){
        if(cmd[i] == ';'){
            conn_order.push_back(1);
        }
        else if(cmd[i] == '&' && cmd[i+1] == '&'){
            conn_order.push_back(2);
            ++i;
        }
        else if(cmd[i] == '|' && cmd[i+1] == '|'){
            conn_order.push_back(3);
            ++i;
        }
    }
}

int main()
{
    char *cmd = 0;
    while(1){
        
        cmd = readCmd(); 
        if(cmd == "exit") exit(0);
        
        if(!isEmpty(cmd) && !isComment(cmd)){
            
            char ** cmdlist = new char *[cmdSize(cmd)];
            tok_comment(cmd);
            readOrder(cmd);
            //display_v(conn_order);
            int list_size = tok_conn(cmdlist,cmd);
            tok_space(cmdlist, list_size);
                                 
        }
        words = 0;
        conn_order.clear(); 
    }
    free(cmd);
    return 0;
} 
