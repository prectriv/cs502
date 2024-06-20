#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

#define MAX_THREADS 15

using namespace std;

// Mutex for protecting shared data
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Shared data
int badFiles = 0;
int directories = 0;
int regularFiles = 0;
int specialFiles = 0;
int totalRegularFileBytes = 0;
int textFiles = 0;
int totalTextFileBytes = 0;

// Function to check if a file is a text file
bool isTextFile(const char* filename) {
    // Open the file in binary mode and set the file pointer to the end
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        return false;
    }

    // Get the file size
    // tellg() function gets the current position of the file pointer (end of file)
    // The value it returns represents the file size
    streamsize fileSize = file.tellg();
    file.seekg(0, ios::beg);  // Reset the pointer back to the beginning of the file to read it

    // Dynamically allocate buffer based on file size
    char* buffer = new char[fileSize];
    if (!file.read(buffer, fileSize)) {
        // If the file cannot be read, delete the buffer and return false
        delete[] buffer;
        return false;
    }

    // Check if all bytes in the file are printable characters or spaces
    for (streamsize i = 0; i < fileSize; ++i) {
        if (!isprint(buffer[i]) && !isspace(buffer[i])) {
            // If a non-printable character or non-space character is found, delete the buffer and return false
            delete[] buffer;
            return false;
        }
    }

    delete[] buffer;
    return true;
}

void* processFile(void* arg) {
    char* filename = (char*)arg;
    struct stat fileStat;

    if (stat(filename, &fileStat) == -1) {
        pthread_mutex_lock(&mutex);
        ++badFiles;
        pthread_mutex_unlock(&mutex);
        delete[] filename;
        return nullptr;
    }

    bool isDir = S_ISDIR(fileStat.st_mode);
    bool isReg = S_ISREG(fileStat.st_mode);
    bool isText = false;
    int size = fileStat.st_size;

    if (isReg) {
        isText = isTextFile(filename);
    }

    pthread_mutex_lock(&mutex);
    if (isDir) {
        ++directories;
    } else if (isReg) {
        ++regularFiles;
        totalRegularFileBytes += size;
        if (isText) {
            ++textFiles;
            totalTextFileBytes += size;
        }
    } else {
        ++specialFiles;
    }
    pthread_mutex_unlock(&mutex);

    delete[] filename;
    return nullptr;
}

int main(int argc, char* argv[]) {
    bool useThreads = false;
    int maxThreads = 1;

    if (argc >= 2 && string(argv[1]) == "thread") {
        useThreads = true;
        if (argc == 3) {
            maxThreads = stoi(argv[2]);
            if (maxThreads < 1 || maxThreads > MAX_THREADS) {
                cerr << "Max threads must be between 1 and " << MAX_THREADS << endl;
                return 1;
            }
        }
    }

    vector<pthread_t> threads;
    queue<pthread_t> threadQueue;
    string filename;

    while (getline(cin, filename)) {
        char* fname = new char[filename.size() + 1];
        strcpy(fname, filename.c_str());

        if (useThreads) {
            pthread_t thread;
            pthread_create(&thread, nullptr, processFile, fname);
            threadQueue.push(thread);

            // ensure adherence to max thread count
            if ((size_t)maxThreads <= threadQueue.size()) {
                pthread_join(threadQueue.front(), nullptr);
                threadQueue.pop();
            }
        } else {
            processFile((void*)fname);
        }
    }

    // Join threads
    while (!threadQueue.empty()) {
        pthread_join(threadQueue.front(), nullptr);
        threadQueue.pop();
    }

    cout << "Bad Files: " << badFiles << endl;
    cout << "Directories: " << directories << endl;
    cout << "Regular Files: " << regularFiles << endl;
    cout << "Special Files: " << specialFiles << endl;
    cout << "Regular File Bytes: " << totalRegularFileBytes << endl;
    cout << "Text Files: " << textFiles << endl;
    cout << "Text File Bytes: " << totalTextFileBytes << endl;

    return 0;
}
