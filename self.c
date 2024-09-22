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

void close_fds(proc_network *net){
    int channel_num = 0;
    local_id id = get_id_by_pid(net, getpid());
    for (int i = 0; i < net->pcount + 1; ++i){
        for (int j = 0; j < net->pcount + 1; ++j){
            if (i == j) continue;
            if (net->channels[channel_num].src == id && net->channels[channel_num].dst != id){
                // printf("closed %d\n", net->channels[channel_num].fd[0]);
                close(net->channels[channel_num].fd[0]);
            } else if (net->channels[channel_num].src != id && net->channels[channel_num].dst == id){
              // printf("closed %d\n", net->channels[channel_num].fd[1]);
                close(net->channels[channel_num].fd[1]);
            } else {
                // printf("closed %d %d\n", net->channels[channel_num].fd[0],net->channels[channel_num].fd[1]);
                close(net->channels[channel_num].fd[0]);
                close(net->channels[channel_num].fd[1]);
            }
            ++channel_num;
        }
    }
}
