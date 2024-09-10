#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ipc.h"
#include "self.h"

int send(void * self, local_id dst, const Message * msg){

  proc_network *net = (proc_network *) self;

  channel c;
  pid_t self_pid = getpid();

  for (int i = 0; i < net->pcount * net->pcount; ++i){
    if ( net->channels[i].src == self_pid && net->channels[i].dst == dst){
      c = net->channels[i];
      goto channel_found;
    }
  }

  return 1;

  channel_found:
  if (write(c.fd[1], msg + sizeof(MessageHeader), msg->s_header.s_payload_len) == -1){
    return 2;
  } 

  return 0;
}

int send_multicast(void * self, const Message * msg){
  return 0;
}

int receive(void * self, local_id from, Message * msg){
  return 0;
}

int receive_any(void * self, Message * msg){
  return 0;
}
