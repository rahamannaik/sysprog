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

struct clientmsg
{
  u_char msg_type;
  u_char msg_len;
  u_char msg[0];
};


















#endif
