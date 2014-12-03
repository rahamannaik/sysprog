#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef unsigned char u_char;

/* this structure will be used to send any message from client to server.
   msg_type field will tell the type of message and it will have value 1 and 2 and 3.
   1  ->  to join the group
   2  ->  to leave the group
   3  ->  to send the result of task along with task id and group id.

   last field is zero size array. so we can allocate clientmsg at run time according to our need.

 */  

struct clientmsg
{
  u_char msg_type;
  u_char msg_len;
  u_char msg[0];
};



















#endif
