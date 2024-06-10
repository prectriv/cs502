#include <pthread.h>
#include <stdlib.h>

#include <iostream>

#define MAXTHREAD 10

struct msg {
    int iFrom; /* who sent the message (0 .. number-of-threads) */
    int value; /* its value */
    int cnt;   /* count of operations (not needed by all msgs) */
    int tot;   /* total time (not needed by all msgs) */
};

typedef struct msg msg;
using namespace std;

void *threadFunction(void *arg) {
    pthread_detach(pthread_self());

    auto id = arg;
    printf("Thread %d created", id);
    return NULL;
}

void initMailBox(int threadCount) {
    pthread_t thread[threadCount];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    msg mailbox[threadCount];
    int mailboxCount = 0;
    for (int i = 0; i < threadCount; i++) {
        pthread_create(&thread[i], NULL, threadFunction, (void *)i);
    }
}

// SendMsg(int iTo, msg &m);
// RecvMsg(int iTo, msg &m);

int main(int argc, char const *argv[]) {
    int threadCount = atoi(argv[argc - 1]);
    if (threadCount > MAXTHREAD) threadCount = MAXTHREAD;
    initMailBox(threadCount);

    int* from;
    int* to;
    fgets("%d %d", *from, )


    // create threads


    // wait for threads to finish
    //  pthread_join(1, NULL);
    //  send mail

    return 0;
}
