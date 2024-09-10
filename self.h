#ifndef SELF_H
#define SELF_H

#include <stdint.h>
#include <sys/types.h>

typedef struct {
  pid_t src;
  pid_t dst;
  int fd[2];
} channel;

typedef struct {
  int pcount;
  pid_t *pids;
  channel *channels;
} proc_network;



#endif //SELF_H
