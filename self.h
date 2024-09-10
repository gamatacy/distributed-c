#ifndef SELF_H
#define SELF_H

#include <stdint.h>
#include <sys/types.h>

typedef struct {
    int pcount;
    pid_t *pids;
} proc_network;

typedef struct {
  pid_t dst_pid;
  int fd[2];
} channel;

#endif //SELF_H
