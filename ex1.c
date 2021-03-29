//Yair Koskas 325699999
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define DELIMITER " "
#define APOSTROPHES '"' 

typedef struct Node {
    char *name;
    char *state;
} Node;





/**
 * @name del
 * @brief deletes all occurences of a character in a string
 * @param str the string to be modified
 * @param ch the char to remove
 */
void del(char str[100], char ch) {
   int i, j = 0;
   int size;
   char ch1;
   char str1[100];
   strcpy(str1,str);
   for (i = 0; i < 100; i++) {
      if (str1[i] != ch) {
         ch1 = str1[i];
         str[j] = ch1;
         j++;
      }
   }
   str[j] = '\0';
}
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
/**
 * @name cd
 * @brief changing the working directory for the current process
 */ 
void cd(char* arguments[100]) {
    if (arguments[2] != NULL) {
        printf("Too many arguments\n");
        fflush(stdout);
        return;
    }
    char *home = getenv("HOME"); //guranteed to be a valid directory
    char *oldpwd = getenv("OLDPWD");
    //now change to path
    if (!strcmp(arguments[1],"-")) {
        int r = chdir(oldpwd);
        if (r == -1) {
            printf("chdir failed\n");
            fflush(stdout);
        }
    } else if (!strcmp(arguments[1],"~")) {
        int r = chdir(home);
        if (r == -1) {
            printf("chdir failed\n");
            fflush(stdout);
        }
    } else {
        int r = chdir(arguments[1]);
        if (r == -1) {
            printf("chdir failed\n");
            fflush(stdout);
        }
    }
    setenv("OLDPWD",getenv("PWD"),1); //setting the OLDPWD environment variable to be the old pwd
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
        if(!strcmp(command,"\n")) { // special case for empty line
            continue;
        }
        if(!strncmp(command,"echo",4)) { // special case for echo command
            del(command,APOSTROPHES);
        }
        removeLineDelimiter(command);
        parseCommand(command,&file,arguments,&background);
        if(!strcmp(file,"cd")) {
            cd(arguments);
            continue;
        } else if (!strcmp(file,"exit")) {
            exit(0);
        } else if (!strcmp(file,"history")) {

        }
        pid_t pid = fork();
        if (pid == -1) {
            printf("fork failed\n");
            fflush(stdout);
        }
        if (pid == 0) { //we're the child process
            int success = execvp(file,arguments);
            if (success == -1) {
                printf("exec failed\n");
            }
            fflush(stdout);
        } else { //we're the parent process
            if(background) {
                waitpid(-1,&status,WNOHANG);
            } else {
                wait(&status);
            }
        }
    }    
}