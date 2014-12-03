#include "common.h"

int sendall(int s, char *buf, int *len)
{
  int total = 0;        // how many bytes we've sent
  int bytesleft = *len; // how many we have left to send
  int n;

  while(total < *len) 
  {
    n = send(s, buf+total, bytesleft, 0);
    if (n == -1)
    { 
      break; 
    }
    total += n;
    bytesleft -= n;
  }

  *len = total; // return number actually sent here

  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
} 

void *join_group(void *arg)
{
  int sockfd = *((int *)arg);

  int n;
  char buffer[256];
  message *ptr;
  bzero(buffer,256);

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
            *ptr->data = htons(option);

            printf("size of message = %d\n", sizeof(message));
            printf("size of *ptr = %d\n", sizeof(*ptr));
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
        break;

      default:
        printf("\ninvalid option\n");
        continue;
    }

    if(sendall(sockfd, ptr, (sizeof(*ptr) + 2)) == -1)
    {
      perror("ERROR writing to socket");
      exit(1);
    }
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
    {
      perror("ERROR reading from socket");
      exit(1);
    }
    printf("%s\n",buffer);
    bzero(buffer,256);
  }

}

void *task_from_server(void *arg)
{
  int sockfd = *((int *)arg); 
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

  bzero((char *) &serv_addr, sizeof(serv_addr));

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
  pthread_create(&thread_id2, &attr, task_from_server, (void *)&sockfd); 
  pthread_exit(NULL);


  return 0;
}

