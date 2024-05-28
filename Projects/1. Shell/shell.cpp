#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>

auto PROMPT = "==>";

using namespace std;

struct bgt {
    pid_t pid;
    string command;
    long start;
};

vector<bgt> bgTasks;

void stats(long start) {
    struct rusage usage;
    struct timeval end;
    gettimeofday(&end, NULL);
    long end_ms = (end.tv_sec * 1000) + (end.tv_usec / 1000);
    getrusage(RUSAGE_CHILDREN, &usage);
    double userTime = ((usage.ru_utime.tv_sec * 1000) + (usage.ru_utime.tv_usec / 1000));
    double systemTime = ((usage.ru_stime.tv_sec * 1000) + (usage.ru_stime.tv_usec / 1000));
    double wallClockTime = end_ms - start;
    cout << "\n---USAGE STATISTICS---" << endl;
    cout << "CPU System time      : " << systemTime << " milliseconds" << endl;
    cout << "CPU User time        : " << userTime << " milliseconds" << endl;
    cout << "Wall-clock time      : " << wallClockTime << " milliseconds" << endl;
    cout << "Involuntary switches : " << usage.ru_nivcsw << endl;
    cout << "Voluntary switches   : " << usage.ru_nvcsw << endl;
    cout << "Major page faults    : " << usage.ru_majflt << endl;
    cout << "Minor page faults    : " << usage.ru_minflt << endl;
    cout << "Resident set size    : " << usage.ru_maxrss << "KB\n" << endl;
}


void testExit(){
    if (bgTasks.size() > 0) {  // wait for children for finish
        cout << "Waiting for " << bgTasks.size() << " process(es) to finish" << endl;
        for (unsigned long i = 0; i < bgTasks.size(); i++) {
            int status;
            pid_t result = waitpid(bgTasks.at(i).pid, &status, 0);
            if (result > 0) {
                cout << "[" << i + 1 << "] " << bgTasks.at(i).pid << " Completed\n";
                stats(bgTasks.at(i).start);
            }
        }
    }
    exit(0);
}

int run(char* argv[], bool isBg) {
    int pid, status;

    pid = fork();  // spawn child

    if (pid < 0) {
        cerr << "Forking error occurred.\n";
        exit(1);
    } else if (pid) {
        struct timeval start;
        gettimeofday(&start, NULL);
        long st = ((start.tv_sec * 1000) + (start.tv_usec / 1000));
        // parent
        if (isBg) {  // background process
            bgTasks.push_back({pid, (string)argv[0], st});
            cout << "[" << bgTasks.size() << "] " << bgTasks.back().pid << endl;
            return 0;
        } else {  // wait for child to finish
            waitpid(pid, &status, 0);
            stats(st);
        }

    } else {
        // Child process
        if (execvp(argv[0], argv) < 0) {
            cerr << "Execvp error\n";
            exit(1);
        }
    }
    return 0;
}

void shell() {
    // cout << filesystem::current_path() << "> ";
    cout << PROMPT << " ";
    char line[128];

    if (fgets(line, 128, stdin) == NULL) {
        shell();
    }

    // Check for background tasks
    for (long unsigned i = 0; i < bgTasks.size(); i++) {
        int status;
        pid_t result = waitpid(bgTasks.at(i).pid, &status, WNOHANG);
        if (result > 0) {  // child quit
            cout << "[" << i + 1 << "] " << bgTasks.at(i).pid << " Completed\n";
            stats(bgTasks.at(i).start);
            bgTasks.erase(bgTasks.begin() + i);
        }
    }

    if (line[0] == '\n' || line[0] == '\t' || line[0] == ' ') {
        return shell();
    }

    int ctr = 0;
    char* newArgs[33] = {0};

    char* token = strtok(line, " \t\n");
    while (token != NULL) {
        newArgs[ctr++] = token;
        token = strtok(NULL, " \t\n");
    }

    if (strcmp(newArgs[0], "exit") == 0) {
        return testExit();
    } else if (strcmp(newArgs[0], "cd") == 0) {
        chdir(newArgs[1]);
        return shell();
    } else if (strcmp(newArgs[0], "jobs") == 0) {
        for (long unsigned i = 0; i < bgTasks.size(); i++) {
            cout << "[" << i + 1 << "] " << bgTasks.at(i).pid << " " << bgTasks.at(i).command << endl;
        }
        return shell();
    } else if (strcmp(newArgs[0], "set") == 0 && strcmp(newArgs[1], "prompt") == 0){
        if (ctr < 4) {
            cerr << "set prompt = <new prompt>\n";
        } else {
            cout << "Prompt changed to " << newArgs[3] << endl;
            PROMPT = newArgs[3];
            cout << endl;
            return shell();
        }
    }

    bool isBg = false;

    if (strcmp(newArgs[ctr - 1], "&") == 0) {
        newArgs[ctr - 1] = NULL;
        isBg = true;
    } else {
        newArgs[ctr - 1][strlen(newArgs[ctr - 1])] = '\0';
    }

    run(newArgs, isBg);
    shell();
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        shell();

    } else {
        run(argv + 1, false);
    }
}