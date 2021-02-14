#include <unistd.h>
 
#include <string.h>
 
#include <stdlib.h>
 
#include <stdio.h>
 
#include <sys/wait.h>
#define MAXLINE 80

int main(int argc, char* argv[])
{
    int n;
    int fd[2];
    pid_t pid;
    char line[MAXLINE];
    char st[] = "hello word";
    if(pipe(fd) < 0) {
        perror("pipe");
        exit(1);
    }
    if(pid = fork() < 0) {
        perror("fork");
        exit(1);
    }
    if(pid > 0) {
        close(fd[0]);
        write(fd[1], st, strlen(st));
        //wait(NULL);
        return 0;
        
    } else {
        close(fd[1]);
        n = read(fd[0], line, sizeof(line));
       // printf("1111");
       // write(STDOUT_FILENO, line, n);
       printf("ddd\n");
       printf("%s, %d\n", line, n);
        
    }
    return 0;

}