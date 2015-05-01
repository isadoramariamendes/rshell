#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <iomanip>
#include <string.h>

using namespace std;


void printPermissions(int protec) {
    if (S_ISDIR(protec) != 0) cout << "d";
    else if (S_ISLNK(protec) != 0) cout << "l";
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
    cout << "\t" << setw(5) << s.st_nlink;
    cout << "\t" << setw(5) << s.st_uid;
    cout << "\t" << setw(5) << s.st_gid;
    cout << "\t" << setw(10) << s.st_size;
    cout << "\t" << setw(15) << s.st_mtime;
    cout << "\t";
}

void printStr(char *c) {
    for (int i = 0; c[i] != '\0'; ++i) {
        cout << c[i];
    }
}

void changeColor(short background, int protec){
    if(background == 1) cout << "\x1b[47m";
    if(S_ISDIR(protec)) cout << "\x1b[34m";
    else if(protec & S_IXUSR) cout << "\x1b[32m";
}

void resetColor(){
    cout<<"\x1b[0m";
}

void print_time(vector<string> &v) {
    int total = 0;
    for (unsigned i = 0; i < v.size(); ++i) {
        struct stat s;
        if (lstat(v.at(i).c_str(), &s) == -1) {
            perror(string("ERROR(" + v.at(i) + ")").c_str());
        }
        else {
            total += s.st_blocks*512;
        }
    }
    total = total != 0 ? total / 512: 0;
    cout << "total " << total <<endl;
}

void print_aFlag(vector<string> &v, vector<string> &n) {
    for (unsigned i = 0; i < v.size(); ++i) {
        struct stat s;
        if (lstat(v.at(i).c_str(), &s) == -1) {
            perror(string("ERROR lstat(" + v.at(i) + ")").c_str());
        }
        else {
            if (v.at(i).find('.') == 0) {//axou ponto
                changeColor(1, s.st_mode);
                cout << n.at(i);
            }
            else {
                changeColor(0, s.st_mode);
                cout << n.at(i);
            }
            resetColor();
            cout << " ";
        }
    }
}

void print_alFlag(vector<string> &v, vector<string> &n) {
    for (unsigned i = 0; i < v.size(); ++i) {
        struct stat s;
        stat(v.at(i).c_str(), &s);
        if (lstat(v.at(i).c_str(), &s) == -1) {
            perror(string("ERROR: lstat(" + v.at(i) + ")").c_str());
        }
        else {
            resetColor();
            printPermissions(s.st_mode);
            printInformation(s);
            if (v.at(i).find('.') == 0) {//axou ponto
                changeColor(1, s.st_mode);
                cout << n.at(i);
                
            }
            else {
                changeColor(0, s.st_mode);
                cout << n.at(i);
            }
            resetColor();
            cout << endl;
        }
    }
}

int main(int argc, char **argv)
{
    //beggining of flags checking
    char dir[] = ".";
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
            }
        }
    }
    if (dir_count == 0) {
        argv[0] = dir;
        ++dir_count;
    }
    //end of flags checking
    
    vector <string> names;
    vector <string> dirs;

    for (int k = 0; k < dir_count; ++k) {
        struct stat s;
        if (lstat(argv[k], &s) < 0) {
            perror("lstat");
            cout << endl;
        }
        else {
            DIR *dirp = opendir(argv[k]);
            if (dirp == NULL) { //is not dir
                resetColor();
                perror("opendir");
                cout << endl;

            }
            else {
                dirent *direntp;
                if (strcmp(argv[k], ".") != 0) {
                    resetColor();
                    printStr(argv[k]);//print folders name
                    cout << ":" << endl;
                }
                //creates list of name of files
                while ((direntp = readdir(dirp))) {
                    if (direntp == NULL) {
                        perror("readdir");
                        cout << endl;
                    }
                    else {

                        if (strcmp(argv[k], ".") == 0) {
                            if (!flag_a) { //exclude hidden files from the list
                                if (direntp->d_name[0]!='.') {
                                    string curr_str(direntp->d_name);
                                    names.push_back(curr_str);
                                    dirs.push_back(curr_str);

                                }
                            }
                            else { //list with --all files
                                string curr_str(direntp->d_name);
                                names.push_back(curr_str);
                                dirs.push_back(curr_str);

                            }
                            
                        }
                        else {

                            if (!flag_a) { //exclude hidden files from the list
                                if (direntp->d_name[0]!='.') {
                                    string curr_str = string(direntp->d_name);
                                    string dir = "/" + string(curr_str);
                                    names.push_back(string(curr_str));
                                    dirs.push_back(argv[k] + dir);
                                }
                            }
                            else { //list with --all files
                                string curr_str = string(direntp->d_name);
                                string dir = "/" + string(curr_str);
                                names.push_back(string(curr_str));
                                dirs.push_back(argv[k] + dir);
                            }
                        }

                    }
                }
                if (!flag_l) { //flag a
                    resetColor();
                    print_aFlag(dirs, names);
                    resetColor();
                }
                else {// flag al or l, dependes on the contents of "names"
                    resetColor();
                    print_time(dirs);
                    print_alFlag(dirs, names);
                    resetColor();
                }
                if (closedir(dirp) == -1) perror("closedir");
                cout << endl;
            }
            names.clear();
            dirs.clear();
        }
        
    }
    return 0;
}
