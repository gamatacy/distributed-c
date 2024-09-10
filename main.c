#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "self.h"

proc_network net;

void child_handler(){
    printf("Child process started with pid %d\n", getpid());

    channel channels[net.pcount];

    for (int i = 0; i < net.pcount; i++) {

        if (net.pids[i] == getpid()) continue;

        if (pipe(channels[i].fd) == -1) {
            printf(" Channel %d->%d failed\n", getpid(), net.pids[i]);
        } else{
            channels[i].dst_pid = net.pids[i];
            printf(" Channel %d->%d created, fd: %d %d\n", getpid(),
                   channels[i].dst_pid, channels[i].fd[0], channels[i].fd[1]);
        }

    }

}

int main(int argc, char *argv[]){

    if (argc < 3) {
        perror("Specify process count via -p flag\n");
        return 1;
    }

    if (strcmp(argv[1], "-p") != 0) {
        perror("Specify process count via -p flag\n");
        return 1;
    }

    net.pcount = strtol(argv[2], NULL, 10);
    net.pids = malloc(net.pcount * sizeof(pid_t));

    for (int i = 0; i < net.pcount; i++) {
        net.pids[i] = fork();

        if (net.pids[i] == -1) {
            perror("Fork failed\n");
            abort();
        }
        else if (net.pids[i] == 0) {
            child_handler();
            exit(0);
        }

    }

    wait(NULL);

    return 0;
}
