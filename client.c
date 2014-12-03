#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void *join_group(void *arg)
{
  int sockfd = *((int *)arg);

  int n;
  char buffer[256];
  bzero(buffer,256);

  while(1)
  {
    unsigned int option;
    printf("select 1 to join a group\n");
    printf("select 2 to send message to a group\n");
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
            snprintf(buffer, 15,"join groupid %d", option);
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

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
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

