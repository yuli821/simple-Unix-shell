#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFF_SIZE 1000
#define WORD_SIZE 101

int readcmd(char* cmd);
int parsecmd(char* cmd, char** parsedcmd);
void freespace(char** parsedcmd);

int main(int argc, char **argv){
    char buff[BUFF_SIZE];
    char* cmd[WORD_SIZE];

    while(1) {
        memset(buff, '\0', BUFF_SIZE*sizeof(char));
        memset(cmd, NULL, WORD_SIZE*sizeof(char*));
        printf("$ ");
        if(readcmd(buff) == 1) {
            buff[0] = '\0';
            continue;
        }
        if(buff[0] == '\0') {
            continue;
        }
        if(parsecmd(buff, cmd) == 1) {
            buff[0] = '\0';
            freespace(cmd);
            continue;
        }
        if(builtin(cmd) == 0) {
            execute(cmd);
        }
        freespace(cmd);
    }

    return 0;
}

void freespace(char** parsedcmd){
    int i = 0;
    if(parsedcmd == NULL) {
        return;
    }
    while(parsedcmd[i] != NULL) {
        free(parsedcmd[i]);
        i++;
    }
}

int readcmd(char* cmd){
    char c;
    int index = 0;
    int checksinglequote = 0;
    int checkdoublequote = 0;
    
    while(1) {
        if(index == BUFF_SIZE) {
            fprintf(stderr, "Exceed maximum characters(1000)\n");
            return 1;
        }
        c = getchar();
        if(c == EOF || c == 0x0A) {
            if(checksinglequote == 1 || checkdoublequote == 1) {
                fprintf(stderr,"Mismatched quotes.\n");
                return 1;
            }
            cmd[index] = '\0';
            return 0;
        }
        else if (c == '\"') {
            if(checksinglequote == 0){
                checkdoublequote = (checkdoublequote + 1) % 2;
            }
            cmd[index] = c;
        }
        else if (c == '\'') {
            if(checkdoublequote == 0) {
                checksinglequote = (checksinglequote + 1) % 2;
            }
            cmd[index] = c;
        }
        else {
            cmd[index] = c;
        }
        index++;
    }
}

int parsecmd(char* cmd, char** parsedcmd) {
    char* temp = malloc(100 * sizeof(char));
    temp[0] = '\0';
    int index = 0, index_cmd = 0, index_p = 0;
    int singlequote = 0, doublequote = 0;
    if(cmd == NULL) {
        return 1;
    }
    while(cmd[index_cmd] != '\0') {if (cmd[index_cmd] == '\"') {
            if (singlequote == 0) {
                doublequote = (doublequote+1) % 2;
            }else{
                temp[index] = cmd[index_cmd];
                index++;
            }
        }
        else if (cmd[index_cmd] == '\'') {
            if(doublequote == 0) {
                singlequote = (singlequote+1) % 2;
            }else{
                temp[index] = cmd[index_cmd];
                index++;
            }
        }
        else if(singlequote) {
            temp[index] = cmd[index_cmd];
            index++;
        }
        else if(doublequote) {
            temp[index] = cmd[index_cmd];
            index++;
        }
        else if(cmd[index_cmd] == ' ') {
            if(temp[0] != '\0') {
                char* word = malloc(100*sizeof(char));
                memset(word, '\0', 100 * sizeof(char));
                temp[index] = '\0';
                strcpy(word, temp);
                parsedcmd[index_p] = word;
                index_p++;
                temp[0] = '\0';
                index = 0;
            }
        }
        else {
            temp[index] = cmd[index_cmd];
            index++;
        }
        index_cmd++;
    }
    if(temp[0] != '\0') {
        char* word = malloc(100*sizeof(char));
        memset(word, '\0', 100 * sizeof(char));
        temp[index] = '\0';
        strcpy(word, temp);
        parsedcmd[index_p] = word;
        index_p++;
    }
    free(temp);
    return 0;
}

int execute(char** arg){
    pid_t pid;
    int status;
    pid = fork();
    if(pid == 0) {
        if(execvp(arg[0], arg) == -1){
            perror("error: ");
            exit(EXIT_FAILURE);
        }
    }
    else if(pid < 0){
        fprintf(stderr, "Fork fails\n");
        return 1;
    }
    else {
        wait(&status);
        if(WEXITSTATUS(status))
        fprintf(stderr, "Exit status: %d\n",WEXITSTATUS(status));
    }
}

int builtin(char** arg) {
    char* builtincmd[2] = {"cd","exit"};
    int index = 0;
    int i;
    for(i = 0 ; i < 2; i++) {
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
