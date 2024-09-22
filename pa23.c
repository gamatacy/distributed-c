#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

#include "ipc.h"
#include "self.h"
#include "pa1.h"
#include "common.h"
#include "banking.h"

void transfer(void * parent_data, local_id src, local_id dst,
              balance_t amount)
{
    
}

proc_network net;
FILE *_events_log;
FILE *_pipes_log;

int child_handler(){

    close_fds(&net);
    
    for(int i = 0; i < 5; ++i){
        printf("time: %d\n" , get_physical_time());
        sleep(1);
    }

    Message *msg = create_message(
        MESSAGE_MAGIC,
        0,
        STARTED,
        time(NULL)
    );
    msg->s_header.s_payload_len = sprintf(msg->s_payload, log_started_fmt, get_id_by_pid(&net, getpid()), getpid(), getppid());

    send_multicast(&net, msg); 
    sleep(1);

    while (receive_any(&net, msg) == 0) {
        printf("%s", msg->s_payload);
        fprintf(_events_log, "%s", msg->s_payload);
    }

    fprintf(_events_log, log_received_all_started_fmt, get_id_by_pid(&net, getpid()));

    msg->s_header.s_local_time = time(NULL);
    msg->s_header.s_payload_len = sprintf(msg->s_payload, log_done_fmt, get_id_by_pid(&net, getpid()));

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
    
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                net.pcount = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Wrong arg: %c\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    net.pcount = strtol(argv[2], NULL, 10);

    int money[net.pcount];

    for (int i = 0; i < net.pcount; ++i){
        start_money[i] = strtol(argv[i + 3], NULL, 10);
    }

    net.pids = malloc((net.pcount + 1) * sizeof(pid_t)); net.pids[0] = getpid();
    net.ids = malloc((net.pcount + 1) * sizeof(local_id)); net.ids[0] = 0;
    net.channels = malloc((net.pcount + 1) * (net.pcount + 1) * sizeof(channel));

    _events_log = fopen(events_log, "a");
    _pipes_log = fopen(pipes_log, "a");
    
    int channel_num = 0;
    for (int i = 0; i < net.pcount + 1; ++i){

        for (int j = 0; j < net.pcount + 1; ++j){
            if (i == j) continue;

            if (pipe(net.channels[channel_num].fd) == -1) {
                perror("Pipe failed\n");
            } else {
                int flags = fcntl(net.channels[channel_num].fd[0], F_GETFL, 0);
                fcntl(net.channels[channel_num].fd[0], F_SETFL, flags | O_NONBLOCK);
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

    close_fds(&net);

    for (int i = 0; i < net.pcount; ++i) wait(&net.pids[i+1]);

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

    fclose(_events_log);
    fclose(_pipes_log);

    return 0;
}

