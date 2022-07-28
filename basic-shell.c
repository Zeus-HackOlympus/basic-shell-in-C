/**
 * @author      : HackOlympus (st4rk@ubuntu)
 * @file        : basic-shell
 * @created     : Tuesday Jul 26, 2022 22:40:12 MST
 */

#include <stdio.h> 
#include <string.h> 
#include <signal.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <limits.h> 
#include <sys/wait.h> 

#define MAX_ARGC 10

int argc ; 
char *argv[MAX_ARGC] = {NULL};  
int isBackground = 0; 

void print_prompt()
{
    char cwd[PATH_MAX+5];
    getcwd(cwd, PATH_MAX); 
    strcat(cwd,"% ");
    printf("%s",cwd); 
    fflush(NULL); 
}

int take_input()
{
    /* take input and split it into tokens */
    /* returns the number of args passed */ 
    
    char line[PATH_MAX]; // THIS IS A BUG //
    
    if (!fgets(line,sizeof line,stdin)) 
        return 0; 
    line[strcspn(line, "\n")] = 0; // remove trailing new line
    fflush(stdin); 
    /* split input into tokens for argv */ 
    char *token; 
    token = strtok(line," "); 
    int args=0; 
    while (token != NULL && args < MAX_ARGC)   
    {
        if (strcmp(token, "\t") && strcmp(token,"\n")) {
            argv[args++] = token;  
            token = strtok(NULL, " "); 
        }
    }

    return args;     
}

void print_args()
{
    printf("argc: %d\n",argc); 
    printf("Arguments: "); 
    for (int i=0; i < MAX_ARGC && argv[i] != NULL; i++) printf("%s ", argv[i]);  
    puts(""); 
}

int pwd()
{
    /* print working directory */
    /* path max is defined in limits.h */
    char cwd[PATH_MAX];
    if ((getcwd(cwd, PATH_MAX)) != NULL) {
        printf("%s\n",cwd);
    }
    else {
        return -1; 
    }
    return 0; 
}

int whoami()
{
    int uid = getuid(); 
    char *username = getlogin(); 
    printf("%d(%s)\n", uid,username); 
    return 0; 
}

int cd()
{
    if (argv[1] == NULL) {
        return chdir(getenv("HOME")); 
    }
    return chdir(argv[1]); 
}

void null_out_array()
{
    for (int i=0; i < MAX_ARGC; i++) argv[i] = NULL; 
}

int main()
{
    // signal(SIGINT, SIG_IGN);
    while(1) {
        print_prompt(); 
        argc = take_input();
        if (argc == 0) {
            puts("Exiting..."); 
            exit(0); 
        }
        
        // if command has to be run in background
        if (!strcmp(argv[argc-1],"&")) 
            isBackground = 1; 

        //print_args();    
        
        if (!strcmp(argv[0], "pwd")) {
            pwd();
        } else if (!strcmp(argv[0], "whoami")) {
            whoami();
        } else if (!strcmp(argv[0],"cd")){
            cd(); 
        } else {
            pid_t pid = fork(); 
            if (pid == 0) {
                execlp(argv[0],argv[1]);  
            } else {
                waitpid(-1,0,0); 
            }
        }
        fflush(NULL); 
        null_out_array(); 
    }
    return 0; 
}
