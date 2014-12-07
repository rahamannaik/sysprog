#include "common.h"


int main()
{
  int socketHandle;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  const int portNumber = 8080;
  const int connQueue = 8;


  // create socket

  socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

  if(socketHandle < 0)
  {
    close(socketHandle);
    exit(EXIT_FAILURE);
  }

  //  fcntl(socketHandle, F_SETFL, O_NONBLOCK);

  memset(&server_addr, 0, sizeof(server_addr));  

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNumber);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(socketHandle, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0)
  {
    close(socketHandle);
    exit(EXIT_FAILURE);
  }

  listen(socketHandle, connQueue);

  while(1)
  {

    int sockConnection;  // new socket handle to send data to client
    struct sockaddr_in client_addr;
    char buffer[10];
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    int client_addr_len = sizeof(client_addr);

    sockConnection = accept(socketHandle, (struct sockaddr *) &client_addr, (socklen_t *)&client_addr_len);

    if(sockConnection < 0)
    {
      close(socketHandle);
      exit(EXIT_FAILURE);
    }

    int retval = fork();

    if(retval == 0)  // child process to handle each new request
    {

      close(socketHandle);


      while(1)
      {

        printf("sending data to client to find the max of given list of numbers\n");

        FILE *myFile;
        myFile = fopen("Random_Numbers.txt", "r");

        //read file into array
        u_int numberArray[1000];
        int i;

        if (myFile == NULL)
        {
          printf("Error Reading File\n");
          exit (0);
        }

        u_int msg_len = sizeof(message) + 1000 * sizeof(u_int);
        message *ptr = malloc(msg_len);

        u_short g_id = 1;
        u_short data_len = 1000 * sizeof(u_int);

        ptr->msg_type = START_OF_TASK;
        ptr->task_id = 1;
        ptr->group_id = htons(g_id);
        ptr->data_len = htons(data_len);

        for (i = 0; i < 1000; i++)
        {
          fscanf(myFile, "%d,", &numberArray[i] );
          u_int num = htonl(numberArray[i]);
          memcpy((u_int *)&(ptr->data[i * sizeof(u_int)]), &num, sizeof(u_int));

        }

        if(sendall(sockConnection, (char *)ptr, msg_len) == -1)                        
        { 
          perror("ERROR writing to socket");                                   
          exit(1);                                                             
        }
        free(ptr);    

        u_char msg_type;
        u_char task_id;
        u_int max;

        if(recvall(sockConnection, (char *)&msg_type, sizeof(msg_type)) < 0)
        {
          perror("ERROR reading from socket");
          exit(1);
        }
        if(recvall(sockConnection, (char *)&task_id, sizeof(task_id)) < 0)
        {
          perror("ERROR reading from socket");
          exit(1);
        }
        if(recvall(sockConnection, (char *)&g_id, sizeof(g_id)) < 0)
        {
          perror("ERROR reading from socket");
          exit(1);
        }

        g_id = ntohs(g_id);

        if(recvall(sockConnection, (char *)&data_len, sizeof(data_len)) < 0)
        {
          perror("ERROR reading from socket");
          exit(1);
        }

        data_len = ntohs(data_len);

        if(recvall(sockConnection, (char *)&max, sizeof(max)) < 0)
        {
          perror("ERROR reading from socket");
          exit(1);
        }

        max = ntohl(max);

        printf("msg_type = %d\n", msg_type);
        printf("task Id = %d\n", task_id);
        printf("group Id = %d\n", g_id);
        printf("Data length = %d\n", data_len);
        printf("Max value = %u\n", max);


        fclose(myFile);

        printf("want to send more numbers?\n");
        char option;
        scanf("%c", &option);
        if(option == 'y')
          continue;
        else
          break; 
      }

      close(sockConnection);
      exit(EXIT_SUCCESS);
    }

  }  
  close(socketHandle);

  return 0;

}


