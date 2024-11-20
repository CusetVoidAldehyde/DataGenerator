#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
int main(int argc,char **argv){
    for(int i=1;i<argc;i++){
        string opt="g++ ",file=argv[i];
        opt=opt+file+" -o ";
        file[file.size()-1]='e';
        file[file.size()-2]='x';
        file[file.size()-3]='e';
        opt=opt+file+" -O2 -static -std=c++14 -Wl,--stack=1073741824";
        system(opt.c_str());
    }

}