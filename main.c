#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "ipc.h"
#include "self.h"
#include "pa1.h"
#include "common.h"

proc_network net;
FILE *_events_log;
FILE *_pipes_log;

int child_handler(){

    Message *msg = create_message(
        MESSAGE_MAGIC,
        48,
        STARTED,
        time(NULL)
    );
    sprintf(msg->s_payload, log_started_fmt, get_id_by_pid(&net, getpid()), getpid(), getppid());

    send_multicast(&net, msg); 
    sleep(1);

    while (receive_any(&net, msg) == 0) {
        printf("%s", msg->s_payload);
        fprintf(_events_log, "%s", msg->s_payload);
    }

    fprintf(_events_log, log_received_all_started_fmt, get_id_by_pid(&net, getpid()));

    msg->s_header.s_local_time = time(NULL);
    msg->s_header.s_payload_len = 32;
    sprintf(msg->s_payload, log_done_fmt, get_id_by_pid(&net, getpid()));

    send_multicast(&net, msg); 
    sleep(1);

    while (receive_any(&net, msg) == 0) {
        printf("%s", msg->s_payload);
        fprintf(_events_log, "%s", msg->s_payload);
    }  

    fprintf(_events_log, log_received_all_done_fmt, get_id_by_pid(&net, getpid()));

    return 0;
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
    net.pids = malloc((net.pcount + 1) * sizeof(pid_t)); net.pids[0] = getpid();
    net.ids = malloc((net.pcount + 1) * sizeof(local_id)); net.ids[0] = 0;
    net.channels = malloc((net.pcount + 1) * (net.pcount + 1) * sizeof(channel));

    _events_log = fopen(events_log, "w");
    _pipes_log = fopen(pipes_log, "w");

    int channel_num = 0;
    for (int i = 0; i < net.pcount + 1; ++i){

        for (int j = 0; j < net.pcount + 1; ++j){
            if (i == j) continue;

            if (pipe(net.channels[channel_num].fd) == -1) {
                perror("Pipe failed\n");
            } else {
                net.channels[channel_num].src = i;
                net.channels[channel_num].dst = j;
                fprintf(_pipes_log, "Pipe id%d->id%d fd%d:fd%d opened\n", i, j, net.channels[channel_num].fd[0], net.channels[channel_num].fd[1]);
            }

            ++channel_num;
        }

    }

    for (int i = 0; i < net.pcount; ++i) {
        net.pids[i+1] = fork();
        net.ids[i+1] = i+1;

        if (net.pids[i+1] == -1) {
            perror("Fork failed\n");
            abort();
        }
        else if (net.pids[i+1] == 0) {
            net.pids[i+1] = getpid();
            exit(child_handler());
        }
    }

    wait(NULL);

    Message *msg = create_message(
        MESSAGE_MAGIC,
        0,
        STARTED,
        time(NULL)
    );

    while (receive_any(&net, msg) == 0) {
        printf("%s", msg->s_payload);
        fprintf(_events_log, "%s", msg->s_payload);
    }

    channel_num = 0;
    for (int i = 0; i < (net.pcount + 1) * (net.pcount + 1); ++i){
        for (int j = 0; j < (net.pcount + 1) * (net.pcount + 1); ++j){
            if (i == j) continue;
            close(net.channels[channel_num].fd[0]);
            close(net.channels[channel_num].fd[1]);
            fprintf(_pipes_log, "Pipe id%d->id%d fd%d:fd%d closed\n", i, j, net.channels[channel_num].fd[0], net.channels[channel_num].fd[1]);
            ++channel_num;
        }
    }

    fclose(_events_log);
    fclose(_pipes_log);

    return 0;
}
