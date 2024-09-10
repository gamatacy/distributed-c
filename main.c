#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ipc.h"
#include "self.h"

proc_network net;

void child_handler(){
    printf("Child process started with pid %d\n", getpid());

    Message msg;
    // msg.s_payload = "Hello\n";

    // printf("Send from %d to %d\n", getpid(), net.pids[2]);
    // printf("Send result: %d\n", send(&net, net.pids[2], &msg));
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
    net.channels = malloc(net.pcount * net.pcount * sizeof(channel));

    for (int i = 0; i < net.pcount; ++i) {
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

    for (int i = 0; i < net.pcount; ++i){

        for (int j = 0; j < net.pcount; ++j){
            if ( net.pids[i] == net.pids[j] ) {continue;}

            if (pipe(net.channels[10 * i + j].fd) == -1) {
                printf(" Channel %d->%d failed\n", getpid(), net.pids[i]);
            } else {
                net.channels[10 * i + j].src = net.pids[i];
                net.channels[10 * i + j].dst = net.pids[j];

                printf(" Channel %d->%d created, fd: %d %d\n", net.channels[10 * i + j].src,
                   net.channels[10 * i + j].dst, net.channels[10 * i + j].fd[0], net.channels[10 * i + j].fd[1]);
            }

        }

    }
    
    wait(NULL);

    return 0;
}
