//Yair Koskas 325699999
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define DELIMITER " "
int main() {
    while(1) {
        char command[100];
        char *arguments[100];
        int status;
        printf("$ ");
        fflush(stdout);
        fgets(command,100,stdin);
        for(int i=0;i<100;++i) {
            if(command[i] == '\n') {
                command[i] = '\0';
                break;
            }
        }
        char *token = strtok(command,DELIMITER);
        const char *file = token;
        int index = 0;
        do {
            if (!strcmp(token,"&")) {
                //make the process run in the background
            }
            arguments[index++] = token;
            token = strtok(NULL,DELIMITER);
        } while (token != NULL);
        arguments[index] = NULL;
        pid_t pid = fork();
        if (pid == 0) {
            execvp(file,arguments);
        } else {
            wait(&status);
            if(WIFEXITED(status)) {
                printf("Yay, Normal Execution!\n");
                fflush(stdout);
            }
        }
    }    
}