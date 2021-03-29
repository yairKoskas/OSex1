//Yair Koskas 325699999
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define DELIMITER " "
/**
 * @name removeLineDelimiter
 * @brief removing the '\n' character from the string
 * @param command the string to be modified
 */
void removeLineDelimiter(char command[100]) {
    for(int i=0;i<100;++i) {
        if(command[i] == '\n') {
            command[i] = '\0'; //string delimiter
            break;
        }
    }
}
/**
 * @name parse_command
 * @brief parsing a shell command for execvp use
 * @param command the command to parse
 * @param file a pointer to the string specifing the filename for the execvp function
 * @param arguments a pointer to the vector of arguments used by the execvp function
 * @param background an indicator variable indicating if the command should run in foreground or background
 */
void parseCommand(char command[100],char **file, char* arguments[100],int *background) {
    char *token = strtok(command,DELIMITER);
    *file = token;
    *background = 0;
    int index = 0;
    do {
        if (!strcmp(token,"&")) {
            *background = 1;
        break;
        }
        arguments[index++] = token;
        token = strtok(NULL,DELIMITER);
    } while (token != NULL);
    arguments[index] = NULL;
}

void cd(const char* path) {
    char *pwd = getenv("PWD"); //guranteed to be a valid directory
    //now change to path
}

int main() {
    while(1) {
        char command[100];
        char *arguments[100];
        int status;
        int background;
        char *file;
        printf("$ ");
        fflush(stdout);
        fgets(command,100,stdin);
        removeLineDelimiter(command);
        parseCommand(command,&file,arguments,&background);
        pid_t pid = fork();
        if (pid == 0) {
            if(!background) {
                printf("%s\n",file);
                execvp(file,arguments);
            } else {
                //implement background work
            }
        } else {
            wait(&status);
            if(WIFEXITED(status)) {
                printf("Yay, Normal Execution!\n");
                fflush(stdout);
            }
        }
    }    
}