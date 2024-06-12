#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAXTHREAD 10
#define RANGE 1
#define ALLDONE 2

struct msg {
    int iFrom;  // sender of message (0... number of threads)
    int value;  // its value
    int cnt;    // count of operations (not needed by all msgs)
    int tot;    // total time (not needed by all msgs)
};

typedef struct msg msg;

msg mailboxes[MAXTHREAD + 1];
pthread_t threads[MAXTHREAD];
sem_t sSem[MAXTHREAD + 1];
sem_t rSem[MAXTHREAD + 1];
sem_t qSem;  // Semaphore to signal messages in the queue

typedef struct {
    msg message;
    int iTo;
} queuedMsg;

queuedMsg queue[MAXTHREAD * 100];  // Queue for undelivered messages
int queue_start = 0, queue_end = 0;

void SendMsg(int iTo, msg *pMsg) {
    sem_wait(&sSem[iTo]);  // wait for mailbox to be available
    mailboxes[iTo] = *pMsg;
    sem_post(&rSem[iTo]);  // release the mailbox semaphore
}

int NBSendMsg(int iTo, msg *pMsg) {
    if (sem_trywait(&sSem[iTo]) == 0) {  // attempt to lock semaphore without blocking
        mailboxes[iTo] = *pMsg;
        sem_post(&rSem[iTo]);  // release the mailbox semaphore
        return 0;              // message placed successfully
    }
    return -1;  // failed to place message
}

void RecvMsg(int iFrom, msg *pMsg) {
    sem_wait(&rSem[iFrom]);  // wait for message
    *pMsg = mailboxes[iFrom];
    sem_post(&sSem[iFrom]);  // release the mailbox semaphore
}

void *adder(void *arg) {
    int index = *((int *)arg);
    free(arg);  // Free the dynamically allocated memory

    msg message;
    int sum = 0;
    int count = 0;
    time_t start_time = time(NULL);

    while (1) {
        RecvMsg(index, &message);

        if (message.value < 0) {
            break;
        }

        sum += message.value;
        ++count;
        sleep(1);
    }

    time_t end_time = time(NULL);
    message.iFrom = index;
    message.value = sum;
    message.cnt = count;
    message.tot = (int)(end_time - start_time);

    SendMsg(0, &message);
    pthread_exit(NULL);
}

void *resend(void *arg) {
    while (1) {
        sem_wait(&qSem);                      // wait for messages in the queue
        if (queue_start == queue_end) break;  // Exit if queue is empty

        queuedMsg qm = queue[queue_start];
        if (NBSendMsg(qm.iTo, &qm.message) == 0) {
            queue_start = (queue_start + 1) % (MAXTHREAD * 100);
        } else {
            sem_post(&qSem);  // Re-signal if still undelivered
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <numOfThreads> [nb]\n", argv[0]);
        exit(1);
    }

    int numThreads = atoi(argv[1]);
    int useNBSend = (argc == 3 && strcmp(argv[2], "nb") == 0);

    if (numThreads < 1 || numThreads > MAXTHREAD) {
        printf("Thread count must between 1 & %d.\n", MAXTHREAD);
        exit(1);
    }

    for (int i = 0; i <= numThreads; i++) {
        sem_init(&sSem[i], 0, 1);
        sem_init(&rSem[i], 0, 0);
    }
    sem_init(&qSem, 0, 0);  // Initialize the queue semaphore

    for (int i = 0; i < numThreads; i++) {
        int *arg = (int *)malloc(sizeof(int));
        *arg = i + 1;
        pthread_create(&threads[i], NULL, &adder, arg);
    }

    if (useNBSend) {
        pthread_t resend_thread;
        pthread_create(&resend_thread, NULL, &resend, NULL);
    }

    // get data from the file INPUT.txt.
    // NOTE: this file uses the example strings provided by the assignment doc.
    FILE *file = fopen("INPUT.txt", "r");
    if (file == NULL) {
        printf("File not found\n");
        exit(1);
    }

    int thread, value;

    while (fscanf(file, "%d %d", &value, &thread) > 0) {
        msg m;
        m.iFrom = 0;
        m.value = value;
        if (useNBSend) {
            if (NBSendMsg(thread, &m) == -1) {
                queue[queue_end].message = m;
                queue[queue_end].iTo = thread;
                queue_end = (queue_end + 1) % (MAXTHREAD * 100);
                sem_post(&qSem);  // Signal there is a message in the queue
            }
        } else {
            SendMsg(thread, &m);
        }
    }
    fclose(file);

    if (useNBSend) {
        while (queue_start != queue_end) {
            queuedMsg qm = queue[queue_start];
            if (NBSendMsg(qm.iTo, &qm.message) == 0) {
                queue_start = (queue_start + 1) % (MAXTHREAD * 100);
            } else {
                sem_post(&qSem);  // Re-signal if still undelivered
            }
        }
    }

    for (int i = 1; i <= numThreads; i++) {
        msg m;
        m.iFrom = 0;
        m.value = -1;
        if (useNBSend) {
            while (NBSendMsg(i, &m) != 0) {
                sem_post(&qSem);  // Re-signal if still undelivered
            }
        } else {
            SendMsg(i, &m);
        }
    }

    msg received;
    for (int i = 1; i <= numThreads; i++) {
        RecvMsg(0, &received);  // receive ALLDONE
        printf("The result from thread %d is %d from %d operations during %d secs.\n",
               received.iFrom, received.value, received.cnt, received.tot);
    }

    for (int i = 0; i <= numThreads; i++) {
        sem_destroy(&sSem[i]);
        sem_destroy(&rSem[i]);
    }
    sem_destroy(&qSem);

    return 0;
}
