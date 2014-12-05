#include "common.h"

void *join_group(void *arg)
{
  int sockfd = *((int *)arg);

  int n;
  char buffer[256];
  message *ptr;
  int data_len, msg_len;
  memset(buffer,0,256);

  while(1)
  {
    unsigned int option;
    printf("select 1 to join a group\n");
    printf("select 2 to send message to a group(Max 255 character long)\n");
    printf("Enter the option: ");

    if(fgets(buffer,255,stdin))
    {
      if(sscanf(buffer,"%d", &option) != 1)
      {
        printf("Please enter valid option\n");
        continue;
      }
    }
    else
    {  
      continue;
    }

    switch(option)
    {
      case 1:
      enter_groupid:
        printf("Please enter the groupid \n");
        if(fgets(buffer,255,stdin))
        {
          if(sscanf(buffer,"%d", &option) == 1)
          {
            ptr = malloc(sizeof(message) + 2); 
            ptr->msg_type = JOIN_GROUP;
            ptr->data_len = htons(2);
            *(ptr->data) = htons(option);
            msg_len = sizeof(message) + 2;
          }
          else
          {
            printf("\nInvalid groupid\n");
            goto enter_groupid;
          }
        }
        else
          continue;
        break;

      case 2:
        printf("Please enter the message: ");
        fgets(buffer,255,stdin);
        data_len = strlen(buffer);
        msg_len = sizeof(message) + data_len;
        ptr = malloc(sizeof(msg_len)); 
        ptr->msg_type = BROADCAST_MSG;
        ptr->data_len = htons(data_len);
        memcpy(ptr->data, buffer, data_len);

        break;

      default:
        printf("\ninvalid option\n");
        continue;
    }

    if(sendall(sockfd, (char *)ptr, msg_len) == -1)
    {
      perror("ERROR writing to socket");
      exit(1);
    }
    free(ptr);
  }

}

int find_max_number(int sockfd)
{
  u_short data_len;

  if(recvall(sockfd, (char *)&data_len, sizeof(data_len)) < 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }
  data_len = ntohs(data_len);

  int data;
  int max = INT_MIN;

  for(int i = 0; i < data_len / sizeof(int); i++)
  {

    if(recvall(sockfd, (char *)&data, sizeof(data)) < 0)
    {
      perror("ERROR reading from socket");
      exit(1);
    }
    data = ntohl(data);

    max = (max < data) ? data : max;
  }
  return max;
}

void *task_from_server(void *arg)
{
  int sockfd = *((int *)arg); 
  int max;
  u_short data_len;
  u_int msg_len;

  u_char task_id;
  u_short group_id;

  if(recvall(sockfd, &task_id, sizeof(task_id)) < 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }

  if(recvall(sockfd, (char *)&group_id, sizeof(group_id)) < 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }

  group_id = ntohs(group_id);

  if(task_id == 1)  // finding max element from given set of numbers.
  {
    max = find_max_number(sockfd);
    data_len = sizeof(max);
    msg_len = sizeof(message) + data_len;

  }
  
  message *ptr = malloc(msg_len);
  ptr->msg_type = REPLY_FROM_CLIENT;
  ptr->task_id = task_id;
  ptr->group_id = htons(group_id);
  ptr->data_len = htons(data_len);
  *(ptr->data) = htonl(max);


  if(sendall(sockfd, (char *)ptr, msg_len) == -1)
  {
    perror("ERROR writing to socket");
    exit(1);
  }
  free(ptr);

  pthread_exit(NULL);


}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  pthread_t thread_id1, thread_id2;
  pthread_attr_t attr;



  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  /* Create a socket point */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
  {
    perror("ERROR opening socket");
    exit(1);
  }
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }

  memset((char *) &serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;

  bcopy((char *)server->h_addr, 
      (char *)&serv_addr.sin_addr.s_addr,
      server->h_length);
  serv_addr.sin_port = htons(portno);

  /* Now connect to the server */
  if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
  {
    perror("ERROR connecting");
    exit(1);
  }	

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_create(&thread_id1, &attr, join_group, (void *)&sockfd); 

  u_char msg_type;
  n = recvall(sockfd, &msg_type, sizeof(msg_type));
  if (n < 0) 
  {
    perror("ERROR reading from socket");
    exit(1);
  }

  if(START_OF_TASK == msg_type)
  {
    pthread_create(&thread_id2, &attr, task_from_server, (void *)&sockfd); 

  }
  pthread_exit(NULL);


  return 0;
}

