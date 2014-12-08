#include "common.h"

void *join_group(void *arg)
{
  int *sockfd = (int *) (arg);

  printf("sock fd in thead is = %d\n", *sockfd);

  int n;
  char buffer[256];
  message *ptr;
  int data_len, msg_len;
  memset(buffer,0,256);

  while(1)
  {
    unsigned int option;
    unsigned int grp_id;
    printf("select 1 to join a group\n");
    printf("select 2 to send message to a group(Max 255 character long)\n");
    printf("select 3 to leave a group\n");
    printf("Enter the option: ");

    if(fgets(buffer,255,stdin))
    {
      if(sscanf(buffer,"%d", &option) != 1)
      {
        printf("Please enter valid option : ");
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
      case 3:
      enter_groupid:
        printf("Please enter the groupid : ");
        if(fgets(buffer,255,stdin))
        {
          if(sscanf(buffer,"%d", &grp_id) == 1)
          {
            ptr = malloc(sizeof(message)); 

            if(ptr == NULL)
            {
              perror("Error: not able to allocate memory");
              exit(1);
            }

            if(option == 1)
            {
              ptr->msg_type = JOIN_GROUP;
            }
            else
            {
              ptr->msg_type = LEAVE_GROUP;
            }

            msg_len = sizeof(message);
            u_short g_id = (u_short)grp_id;
            ptr->group_id = htons(g_id);
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

        if(ptr == NULL)
        {
          perror("Error: not able to allocate memory");
          exit(1);
        }
        ptr->msg_type = BROADCAST_MSG;
        ptr->data_len = htons(data_len);
        memcpy(ptr->data, buffer, data_len);

        break;

      default:
        printf("\ninvalid option\n");
        continue;
    }

    if(sendall(*sockfd, (char *)ptr, msg_len) == -1)
    {
      perror("ERROR writing to socket");
      exit(1);
    }
    free(ptr);
  }

}

u_int calculate_digits(u_int num)
{
  u_int count = 0;

  while(num)
  {

    num=num/10;
    count++;
  }
  return count;
}

u_int find_max_number(int sockfd)
{
  u_short data_len;

  if(recvall(sockfd, (char *)&data_len, sizeof(data_len)) < 0)
  {
    perror("ERROR reading from socket");
    printf("%s:%d, Can't read from Socket : %d\n", __func__, __LINE__, sockfd);
    exit(1);
  }
  data_len = ntohs(data_len);

  printf("%s:%d, Data Len : %d\n", __func__, __LINE__, data_len);

  u_int data;
  u_int max = 0;

  data_len = data_len - sizeof(message);

  char *data_ptr = malloc(data_len);


  if(recvall(sockfd, data_ptr, data_len) < 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }

  //printf("%s\n", data_ptr); 

  u_int digits;
  u_int total_digit = 0;

  while(sscanf(data_ptr, "%d", &data))
  {
    digits = calculate_digits(data);
    digits++;
    total_digit += digits;
    if(total_digit >= data_len)
    {
      break;
    }
    data_ptr = data_ptr + digits;

    max = (max < data) ? data : max;

  }
  

  return max;
}


u_int word_count(int sockfd)
{
  u_short data_len;

  if(recvall(sockfd, (char *)&data_len, sizeof(data_len)) < 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }

  data_len = ntohs(data_len);


  u_char *data_ptr = malloc(data_len);

  u_int count = 0;

  for(int i = 0; i < data_len; i++)
  {
    if(data_ptr[i] == ' ')
    {
      count++;
    }
  }

  count = htonl(count);
  return count;
}


void task_from_server(int sockfd)
{
  u_int max;
  u_int count_words;
  u_short data_len;
  u_int msg_len;
  message *ptr;

  char task_id;
  u_short group_id;


  if(recvall(sockfd, &task_id, sizeof(task_id)) < 0)
  {
    perror("ERROR reading from socket");
    printf("%s:%d, Can't read from Socket : %d\n", __func__, __LINE__, sockfd);
    exit(1);
  }

  printf("%s:%d, Task Id : %d\n", __func__, __LINE__, task_id);

  if(recvall(sockfd, (char *)&group_id, sizeof(group_id)) < 0)
  {
    perror("ERROR reading from socket");
    printf("%s:%d, Can't read from Socket : %d\n", __func__, __LINE__, sockfd);
    exit(1);
  }

  group_id = ntohs(group_id);

  printf("%s:%d, Group Id : %d\n", __func__, __LINE__, group_id);

  if(ptr == NULL)
  {
    perror("Error: not able to allocate memory");
    exit(1);
  }

  if(task_id == 1)  // finding max element from given set of numbers.
  {
    max = find_max_number(sockfd);
    data_len = sizeof(max);
    msg_len = sizeof(message) + data_len;
    u_int max1 = htonl(max);
    ptr = malloc(msg_len);
    memcpy((u_int *)ptr->data, &max1, sizeof(u_int));
  }

  if(task_id == 2) // couting the words in file chunk
  {
    count_words = word_count(sockfd);
    data_len = sizeof(count_words);
    msg_len = sizeof(message) + data_len;
    count_words = htonl(count_words);
    ptr = malloc(msg_len);
    memcpy((u_int *)ptr->data, &count_words, sizeof(u_int));
  }

  ptr->msg_type = REPLY_FROM_CLIENT;
  ptr->task_id = task_id;
  ptr->group_id = htons(group_id);
  ptr->data_len = htons(data_len);

  if(sendall(sockfd, (char *)ptr, msg_len) == -1)
  {
    perror("ERROR writing to socket");
    exit(1);
  }

  free(ptr);

  return;
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

  //printf("socked fd = %d\n", sockfd);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_create(&thread_id1, &attr, join_group, (void *)&sockfd); 

  while(1)
  {
    //printf("message received form server\n");

    char msg_type;
    n = recvall(sockfd, &msg_type, sizeof(msg_type));
    if (n < 0) 
    {
      printf("ERROR reading from socket.....................");
      printf("%s:%d, Can't read from Socket : %d\n", __func__, __LINE__, sockfd);
      exit(1);
    }

    if(START_OF_TASK == msg_type)
    {
      printf("%s:%d, Task From Server Received : Sock Fd : %d\n", __func__, __LINE__, sockfd);
      task_from_server(sockfd); 
    }

  }
  pthread_exit(NULL);

  return 0;
}

