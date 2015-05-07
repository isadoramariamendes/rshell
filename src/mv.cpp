#include <iostream> // cin, cout
#include <fstream> // ifstream
#include <stdlib.h> // ifstream
#include <stdio.h> // ifstream
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;


bool exist(char * f) {
    struct stat s;
    if(stat(f, &s) == 0) {
        return true;
    }
    return false;
}

bool isFolder(char * f) {
    struct stat s;
    if (stat(f, &s) == 0) {
        if (S_ISDIR(s.st_mode) == 0) {
            return false;
        }
        else {
            return true;
        }
    }
    return false;
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Not enough parameters" << endl;
    }
   
    if (!exist(argv[1])){
        cerr << "ERROR: Input file does not exist." <<endl <<endl;
        exit(1);
    }
    else {
        if (!exist(argv[2])) {
            //rename argv[1] to argv[2]
            if (link(argv[1], argv[2]) == -1) {
                perror("link");
            }
            else {
                if (unlink(argv[1]) == -1) {
                perror("unlink");
                }
            }
            link(argv[1], argv[2]);
            unlink(argv[1]);
        }
        else {
            if (isFolder(argv[2])) {
                //move file into folder
                //link and unlink
                //it will be in folder/file
                string newpath  = string(argv[2]) + "/" + string(argv[1]);
                if (link(argv[1],newpath.c_str()) == -1) {
                    perror("link");
                }
                else {
                    if (unlink(argv[1]) == -1) {
                        perror("unlink");
                    }
                }
                link(argv[1],newpath.c_str());
                unlink(argv[1]);
            }
            else {
                cerr << "ERROR: Destination \"" << argv[2] << "\" is not a directory." << endl;
            }

        }

    }
   return 0;

}
