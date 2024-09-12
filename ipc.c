#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include "ipc.h"
#include "self.h"

int send(void *self, local_id dst, const Message *msg) {

  proc_network *net = (proc_network *)self;

  channel c;
  pid_t self_pid = getpid();
  local_id id = get_id_by_pid(net, self_pid);

  for (int i = 0; i < (net->pcount + 1) * (net->pcount + 1); ++i) {
    if (net->channels[i].src == id && net->channels[i].dst == dst) {
      c = net->channels[i];
      goto channel_found;
    }
  }

  return 1;

channel_found:
  if (write(c.fd[1], msg,
            sizeof(MessageHeader) + msg->s_header.s_payload_len) == -1) {
    return 2;
  }

  return 0;
}

int send_multicast(void *self, const Message *msg) {
  proc_network *net = (proc_network *)self;

  channel c;
  pid_t self_pid = getpid();
  local_id id = get_id_by_pid(net, self_pid);

  int ret = 0;

  for (int i = 0; i < (net->pcount + 1) * (net->pcount + 1); ++i) {
    if (net->channels[i].src == id) {
      c = net->channels[i];
      if (write(c.fd[1], msg,
                sizeof(MessageHeader) + msg->s_header.s_payload_len) == -1) {
        ret = 1;
      }
    }
  }

  return ret;
}

int receive(void *self, local_id from, Message *msg) {

  proc_network *net = (proc_network *)self;

  channel c;
  pid_t self_pid = getpid();
  local_id id = get_id_by_pid(net, self_pid);

  for (int i = 0; i < (net->pcount + 1) * (net->pcount + 1); ++i) {
    if (net->channels[i].src == from && net->channels[i].dst == id) {
      c = net->channels[i];
      goto channel_found;
    }
  }

  return 1;

channel_found:
  if (read(c.fd[0], msg, sizeof(Message)) == -1) {
    return 2;
  }

  return 0;
}

int receive_any(void *self, Message *msg) {
  proc_network *net = (proc_network *)self;

  pid_t self_pid = getpid();
  local_id id = get_id_by_pid(net, self_pid);

  for (int i = 0; i < (net->pcount + 1) * (net->pcount + 1); ++i) {
    if (net->channels[i].dst == id) {
      int flags = fcntl(net->channels[i].fd[0], F_GETFL, 0);
      fcntl(net->channels[i].fd[0], F_SETFL, flags | O_NONBLOCK);
      if (read(net->channels[i].fd[0], msg, sizeof(Message)) > 0) {
        return 0;
      }
    }
  }

  return 1;
}
