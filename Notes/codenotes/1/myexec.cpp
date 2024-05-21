#include <iostream>
using namespace std;
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ; /* environment info */

int main(int argc, char **argv) {
    char *argvNew[5];
    int pid;

    if ((pid = fork()) < 0) { 
        cerr << "Fork error\n";
        exit(1);
    } else if (pid == 0) {
        /* child process */
        argvNew[0] = "/bin/ls";
        argvNew[1] = "-l";
        argvNew[2] = NULL;
        if (execve(argvNew[0], argvNew, environ) < 0) {
            cerr << "Execve error\n";
            exit(1);
        }
    } else {
        /* parent */
        wait(0); /* wait for the child to finish */
    }
}