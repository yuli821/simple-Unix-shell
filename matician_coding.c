#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define BUFF_SIZE 1000
#define WORD_SIZE 101

int main(int argc, char **argv){
    char buff[BUFF_SIZE];
    char* cmd[WORD_SIZE];

    while(1) {
        printf("$ ");
        readcmd(buff);
        parsecmd(buff, cmd);
        if(builtin(cmd) == 0) {
            execute(cmd);
        }
    }

    return 0;
}

void readcmd(char* cmd){
    char c;
    int index = 0;
    char* word;
    
    while(1) {
        if(index == BUFF_SIZE) {
            fprintf(stderr, "Exceed maximum characters(1000)\n");
            return;
        }
        c = getchar();
        if(c == EOF || c == "\n") {
            cmd[index] = NULL;
            return;
        }
        else {
            cmd[index] = c;
        }
        index++;
    }
}

void parsecmd(char* cmd, char** parsedcmd) {
    int index = 0;
    char* word;
    while(word = strsep(cmd, " ") != NULL) {
        if(index == WORD_SIZE) {
            fprintf(stderr, "Exceed maximum arguments(100)\n");
            return;
        }
        parsedcmd[index] = word;
        index++;
    }
    parsedcmd[index] = NULL;
    return;
}

int execute(char** arg){
    pid_t pid;
    int status;
    pid = fork();
    if(pid == 0) {
        if(execvp(arg[0], arg) == -1){
            perror(arg[0]);
            return 1;
        }
    }
    else if(pid > 0){
        waitpid(pid,&status);
        return 0;
    }
    else {
        fprintf(stderr, "Fork fails");
        return 1;
    }
    return 0;
}

int builtin(char** arg) {
    char* builtincmd[2] = {"cd","exit"};
    int index;
    for(int i = 0 ; i < 2; i++) {
        if(strcmp(arg[0],builtincmd[i]) == 0){
            index = i + 1;
            break;
        }
    }
    switch(index){
        case 1:
            if(arg[1] == NULL) {
                fprintf(stderr, "No argument of cd\n");
                return 1;
            }
            if(chdir(arg[1]) == -1){
                perror("cd");
                return 1;
            }
            return 2;
        case 2:
            exit(EXIT_FAILURE);
        default: 
            break;
    }
    return 0;
}
