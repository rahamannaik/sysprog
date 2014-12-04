#ifndef __SERVER_H__

#define __SERVER_H__

#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
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

typedef enum server_error_codes 
{
  STATUS_SUCCESS = 0,
  STATUS_FAILURE,
  STATUS_FILE_READ_ERR,
  STATUS_FILE_WRITE_ERR,
  STATUS_SOCKET_READ_ERR,
  STATUS_SOCKET_WRITE_ERR,
  STATUS_NO_CLI_IN_GRP,
  STATUS_CANT_SEEK
} server_error_codes_e;

server_error_codes_e depth_first_traversal_and_send_data(Node * t, int size_per_client, int filefd /* , pointer to mesg_struct pre-filled with task id, group id */);
server_error_codes_e depth_first_traversal_core_and_send_data( Node * t, int level, int size_per_client, int filefd, off_t *offset /* , pointer to mesg_struct pre-filled with task id, group id */);
server_error_codes_e read_and_send_data (int sock_fd, int size_per_client, int filefd, off_t *offset /* , pointer to mesg_struct pre-filled with task id, group id */ );

#endif
