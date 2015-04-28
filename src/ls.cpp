#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <iomanip>
using namespace std;

struct stat s;

void printStr(char *c) {
    for (int i = 0; c[i] != '\0'; ++i) {
        cout << c[i];
    }
}

void printV(vector<string> &v) {
    for (unsigned i = 0; i < v.size(); ++i) {
        cout << v.at(i) << " ";
    }
}

void printPermissions(int protec) {
    if (S_ISDIR(protec) != 0) cout << "d";
    else cout << "-";
    cout << ((protec & S_IRUSR)?"r":"-");
    cout << ((protec & S_IWUSR)?"w":"-");
    cout << ((protec & S_IXUSR)?"x":"-");
    cout << ((protec & S_IRGRP)?"r":"-");
    cout << ((protec & S_IWGRP)?"w":"-");
    cout << ((protec & S_IXGRP)?"x":"-");
    cout << ((protec & S_IROTH)?"r":"-");
    cout << ((protec & S_IWOTH)?"w":"-");
    cout << ((protec & S_IXOTH)?"x":"-");
}

void printInformation(struct stat s) {
    cout << "   " << setw(10) << s.st_nlink;
    cout << "   " << setw(10) << s.st_uid;
    cout << "   " << setw(10) << s.st_gid;
    cout << "   " << setw(10) << s.st_size;
    cout << "   " << setw(10) << s.st_mtime;
    cout << "   ";
}

int main(int argc, char **argv)
{
    char dir[] = ".";
    vector <string> names;

    if (argc == 1) {
        argv[0] = dir;
    }
    else {
        for(int i = 0; i < argc - 1; ++i) {
            argv[i] = argv[i + 1];
        }
        argc--;
    }
    bool flag_a = false;
    bool flag_l = false;
    bool flag_R = false;
    bool is_flag = false;
    bool is_dir = false;
    int dir_count = 0;
    for (int i = 0; i < argc; ++i) {
        is_flag = false;
        if (argv[i][0] == '-') {
            is_flag = true;
        }
        else {
            is_dir = true;
            argv[dir_count] = argv[i];
            ++dir_count;
        }
        for (int j = 1; argv[i][j] != '\0'; ++j) {
            if (is_flag == true && is_dir == false) {
                if(argv[i][j] == 'a') flag_a = true;
                if(argv[i][j] == 'l') flag_l = true;
                if(argv[i][j] == 'R') flag_R = true;
            }
        }
    }
    if (dir_count == 0) {
        argv[0] = dir;
        ++dir_count;
    }
    
    for (int k = 0; k < dir_count; ++k) {
        DIR *dirp1 = opendir(argv[k]);
        if (dirp1 == NULL) {
            if (!flag_l) {
                printStr(argv[k]);
            }
            else {
                stat(argv[k], &s);
                int protec = s.st_mode;
                printPermissions(protec);
                printInformation(s);
                printStr(argv[k]);
                cout << endl;
            }
        }
        else {
            if (dir_count > 1){
                cout << endl;
                printStr(argv[k]);
                cout << ":" << endl;
            }
            dirent *direntp;
            while ((direntp = readdir(dirp1))) {
                if (direntp == NULL) perror("readdir");
                else {
                    if (!flag_a) {
                        if (direntp->d_name[0]!='.') {
                            string curr_str(direntp->d_name);
                            names.push_back(curr_str);
                        }
                    }
                    else {
                        string curr_str(direntp->d_name);
                        names.push_back(curr_str);
                    }
                }
            }
            if (!flag_l) {
                printV(names);
            }
            else {
                for (unsigned l = 0; l < names.size(); ++l) {
                    stat(names.at(l).c_str(), &s);
                    int protec = s.st_mode;
                    printPermissions(protec);
                    printInformation(s);
                    cout << names.at(l) << endl;
                }
            }
            if (closedir(dirp1) == -1) perror("closedir");
            cout << endl;
        }
        names.clear();
    }
    return 0;
}
