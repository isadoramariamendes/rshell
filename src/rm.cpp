#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <list>
#include <dirent.h>
#include <stdlib.h>
using namespace std;

void recRM(char *pathname) {
    DIR *cDir;
    struct dirent *cDirent;
    list<string> dirs;
    string spath = pathname;

    if((cDir = opendir(pathname)) == NULL) {
        perror("opendir");
        exit(0);
    }
    while((cDirent = readdir(cDir)) != 0) {
        string relPath = spath;
        if(errno != 0) {
            perror("readdir error");
            break;
        }
        string str = cDirent->d_name;
        if(str == "." || str == "..") {
            continue;
        }
        cout << cDirent->d_name << endl;
        struct stat s;
        relPath += "/" + (string)cDirent->d_name;
        cout << relPath << endl;
        if(stat(relPath.c_str(), &s) == -1) {
            perror("stat");
            break;
        }
        if(s.st_mode & S_IFDIR) {
            dirs.push_back(cDirent->d_name);
        }
        else {
            if(unlink(relPath.c_str()) == -1) {
                perror("unlink");
            }
        }
    }

    for(std::list<string>::iterator i = dirs.begin(); i != dirs.end(); ++i) {
        string dirPath = spath + "/" + (*i);
        recRM((char*) dirPath.c_str());
    }

    for(std::list<string>::iterator i = dirs.begin(); i != dirs.end(); ++i) {
        string dirPath = spath + "/" + (*i);
        rmdir((char*) dirPath.c_str());
    }

}

int main(int argc, char **argv) {
    bool FlagR = false;
    if(argc < 2) {
        cerr << "Not enough parameters" << endl;
        exit(0);
    }
    
    int pos = 1;
    while (pos < argc) {
        if(*(argv[pos]) == '-') {
            int wordPos = 0;
            while(*(argv[pos] + wordPos) != '\0') {
                if(*(argv[pos] + wordPos) == 'r') {
                    FlagR = true;
                }
                ++wordPos;
            }
        }
        ++pos;
    }

    for(int i = 1; i < argc; ++i) {
        if(*(argv[i]) == '-') {
            continue;
        }
        struct stat s;
        if(stat(argv[i], &s) == -1) {
            perror("stat");
            continue;
        }

        if(s.st_mode & S_IFDIR) {
            if(FlagR) {
                recRM(argv[i]);
                rmdir(argv[i]);
            }
            else {
                cerr << argv[i] << " is a folder, you must include the -r flag" << endl;
                continue;
            }
        }

        else if(s.st_mode & S_IFREG) {
            if(unlink(argv[i]) == -1) {
                perror("unlink");
                continue;
            }
        }

    }

    return 0;
}
