#include "self.h"

local_id get_id_by_pid(proc_network *net, pid_t pid) {
  for (int i = 0; i < net->pcount + 1; ++i) {
    if (net->pids[i] == pid)
      return net->ids[i];
  }

  return -1;
}

Message *create_message(uint16_t s_magic, uint16_t s_payload_len,
                        int16_t s_type, timestamp_t s_local_time) {
  Message *message = (Message *)malloc(sizeof(Message));

  message->s_header.s_magic = s_magic;
  message->s_header.s_payload_len = s_payload_len;
  message->s_header.s_type = s_type;
  message->s_header.s_local_time = s_local_time;

  return message;
}
