#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

/* task ID = 1  finding the max value from given list of numbers
 */  

typedef unsigned char u_char;
typedef unsigned short int u_short;
typedef unsigned int u_int;

typedef enum message_type {
  JOIN_GROUP = 0,
  LEAVE_GROUP,
  BROADCAST_MSG,
  START_OF_TASK,
  TASK_DATA,
  END_OF_TASK,
  ERR_MSG //(Could be used to inform any error message to the clients in the group) 
}message_type_e;

typedef struct message
{
  u_char msg_type;
  u_char task_id;
  u_short group_id;
  u_short data_len;
  u_char data[]; 
}message;

int sendall(int s, char *buf, int len);
int recvall(int s, char *buf, int len);

#endif
