#ifndef SELF_H
#define SELF_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "ipc.h"
#include "banking.h"

typedef struct {
  local_id src;
  local_id dst;
  int fd[2];
} channel;

typedef struct {
  int pcount;
  pid_t *pids;
  local_id *ids;
  channel *channels;
  AllHistory history;
} proc_network;

local_id get_id_by_pid(proc_network *net, pid_t pid);
Message *create_message(uint16_t s_magic, uint16_t s_payload_len, int16_t s_type,
                       timestamp_t s_local_time);
void close_fds(proc_network *net);

#endif // SELF_H
