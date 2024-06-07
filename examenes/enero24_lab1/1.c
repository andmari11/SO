#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>


int setargs(char* args, char* argv[]){

    int i=0;
    int nArgs=0;
    int inicio=1;

    while(args[i]!='\0'){
        if(isspace(args[i])){
            if(argv!=NULL){
                args[i]='\0';
            }
            nArgs++;
            inicio=1;
        }
        else if(inicio){
            if(argv!=NULL){
                argv[nArgs]=&args[i];
            }
            inicio=0;
        }
        i++;
    }
    return nArgs;
}


int main(){

    char* args= strdup("aaaa bbb cc dd");
    char** argv=malloc(setargs(args, NULL));

}