#ifndef __SERVER_H__

#define __SERVER_H__

#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <syslog.h>
#include "patricia.h"

#define MAX_GROUP_SIZE 1000

int logopt = LOG_PID | LOG_INFO | LOG_CONS;
int facility = LOG_USER;
int priority = LOG_ERR | LOG_USER;

typedef struct group_member
{
 struct sockaddr_in client_addr;
 int client_port;
 //char buffer[1000];
 struct group_member *next;
} group_member_t;

Node *group_client_map[MAX_GROUP_SIZE];

#endif
