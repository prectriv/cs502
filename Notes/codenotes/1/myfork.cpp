#include <iostream>
using namespace std;
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pid;
    int i;

    /* print out the arguments */
    cout << "There are " << argc << " arguments:\n";
    for (i = 0; i < argc; i++)
        cout << argv[i] << "\n";

    if ((pid = fork()) < 0) {
        cerr << "Fork error\n";
        exit(1);
    } else if (pid == 0) {
        /* child process */
        for (i = 0; i < 5; i++)
            cout << "child (" << getpid() << ") : " << argv[2] << "\n";
        exit(0);
    } else {
        /* parent */
        for (i = 0; i < 5; i++)
            cout << "parent (" << getpid() << ") : " << argv[1] << "\n";
        exit(0);
    }
}