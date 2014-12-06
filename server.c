#include "server.h"

group_member_t *insert_group_list(int groupid, struct sockaddr_in *client_addr);
void doprocessing (int sock, struct sockaddr_in *client_addr);
int insert_to_patricia(Node *header_of_patricia, int sock_fd);

group_member_t *head = NULL;
group_member_t *group[1000] = {NULL};

group_member_t *insert_group_list(int groupid, struct sockaddr_in *client_addr)
{
 group_member_t *first = group[groupid-1], *grpmember = NULL;
 
 //printf("I am in insert_group_list() func");
 grpmember = (group_member_t *)malloc(sizeof(group_member_t));
 memcpy(&grpmember->client_addr, client_addr, sizeof(struct sockaddr_in));
 grpmember->next = NULL;

 if(first == NULL)
 {
  first = grpmember;
  group[groupid-1] = first;
  //printf("First member in group added\n");
 }
 else
 {
   group_member_t *temp=first;
   while(temp->next != NULL)
   {
    temp = temp->next;
   }
  temp->next = grpmember;
  
  //printf("Subsequent member in group added\n");
 
 }
return grpmember;
}
int main( int argc, char *argv[] )
{
    char *ident = "Server : ";
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    int pid, pid1;

    /* Opening the syslog. */
    openlog(ident, logopt, facility);

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        syslog(priority, "Coudln't open the socket, exiting.");
        closelog();
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         syslog(priority, "Coudln't bind the socket, exiting.");
         closelog();
         exit(1);
    }
    /* Now start listening for the clients, here 
     * process will go in sleep mode and will wait 
     * for the incoming connection
     */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
      
       /* pid1 = fork();
        if (pid1 < 0)
        {
            perror("ERROR on fork");
	    exit(1);
        }
    if (pid1 == 0)  
    {
      if(start_timer(10000, &timer_handler))
      {
        printf("\n timer error\n");
        return(1);
      }
      //while(1);
      //stop_timer();
    }
    else*/
    {

     while (1) 
     {
        newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, (unsigned int *) &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            syslog(priority, "Coudln't accept new connection, exiting.");
            closelog();
            continue;
        }
        /* Create child process */
        pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
	          exit(1);
        }
        if (pid == 0)  
        {
            /* This is the client process */
            close(sockfd);
            doprocessing(newsockfd, &cli_addr);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
     } /* end of while */
    }
}

void doprocessing (int sock, struct sockaddr_in *client_addr)
{
    int n, groupid = 0;
    char buffer[256];
    group_member_t *grpmember = NULL;
    
    bzero(buffer,256);

    while(1)
    {
     n = read(sock,buffer,255);
     if (n < 0)
     {
        perror("ERROR reading from socket");
        exit(1);
     }
      buffer[n]= '\0';
      //printf("Here is the message: %s\n",buffer);
      
      if(!memcmp(buffer, "join groupid", 12))
      {
       //printf("join cmd received\n");
       groupid = atoi(&buffer[13]);
       
       if(!groupid)
       {
        printf("groupid can't be zero. Server exiting...\n");
        /* TODO :: Instead of exiting here, it would be better if we could just ignore this message. */
        exit(0);
       }
       grpmember = insert_group_list(groupid, client_addr);
       insert_to_patricia(group_client_map[groupid], sock);
       
       bzero(buffer,256);
       
       if(grpmember != NULL)
       {
        printf("client joined to group %d\n", groupid);

          n = write(sock,"client joined",13);
          if (n < 0) 
          {
              perror("ERROR writing to socket");
              exit(1);
          }
       }
       else
       {
        //printf("something went wrong while adding\n");
          n = write(sock,"client denied",13);
          if (n < 0) 
          {
              perror("ERROR writing to socket");
              exit(1);
          }
         bzero(buffer,256);
       }
      }
      else
      {
       if(groupid && grpmember != NULL)
       {
         unsigned short offset = 0;
         char s[INET_ADDRSTRLEN];
         printf("Here is the message: %s\n",buffer);
         
         //memcpy(grpmember->buffer+offset, buffer, n);
         //printf("Msg received printing\n"); 
         //printf("The message received from client %s is %s\n", 
          //       inet_ntop(client_addr->sin_family, client_addr->sin_addr, s, sizeof s),
            //     buffer);
         //printf("The message received from client %s:%d is %s\n", inet_ntoa(grpmember->client_addr.sin_addr.s_addr), grpmember->client_addr.sin_port, grpmember->buffer);
         //grpmember->buffer[n]='\0';
         offset += n;
         bzero(buffer,256);
          
          n = write(sock,"I got your message",18);
          if (n < 0) 
          {
              perror("ERROR writing to socket");
              exit(1);
          }
       }
       else
       {
          //break;
          //printf("Client does not belong to any group. So dropping the packet groupid = %d grpmember = %u\n", groupid, grpmember);
          n = write(sock,"Client does not belong to any group. So dropping the packet.", 60);
          if (n < 0) 
          {
              perror("ERROR writing to socket");
              exit(1);
          }
         bzero(buffer,256);
       }
      }
    }  

    //close(sock);
    /*n = write(sock,"Client does not belong to any group. So dropping the packet.", 60);
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }*/
}

int insert_to_patricia(Node *header_of_patricia, int sock_fd)
{
  int status = insert(&header_of_patricia, (DataType)sock_fd);
  depth_first_traversal_and_send_data(header_of_patricia, 200, 0/* , pointer to mesg_struct pre-filled with task id, group id */);
  return status;
}

/*void timer_handler(void)
{
            printf("Timer fired\n");
            display_msgs_per_group();
            stop_timer();
}

void display_msgs_per_group()
{
 int i = 0;
 for(i = 0; i<10; i++)
 {
  group_member_t *temp = group[i]; 
  int j = 1;
  printf("Displaying msgs from groupid %d\n", i+1);
  for(j = 1; temp!=NULL; j++)
  {
   printf("Msgs from client %d is :\n",j);
   printf("%s",temp->buffer);
  }
 }
}*/

server_error_codes_e depth_first_traversal_and_send_data(Node * t, int size_per_client, int filefd /* , pointer to mesg_struct pre-filled with task id, group id */)
{
	if ( !t) return STATUS_NO_CLI_IN_GRP;   // root is null

	Node * current, *next;
  server_error_codes_e status;
  off_t offset = 0;

  filefd = open ("./Random_Numbers.txt", O_RDONLY); 

	current = t;

	next     = t-> leftchild;

	if (  next-> bitpos > t->bitpos) 
	{
		printf("0)Left Branch:bit %d %d\n", 
				next-> bitpos, next->node_count); 
	}
	else
	{
		printf("0)Left:Data %#x %d\n",  
				(next->key&0xff000000u)>>24, 
				next->node_count); 
    status = read_and_send_data(next->key, size_per_client, filefd, &offset /* , pointer to mesg_struct pre-filled with task id, group id */);
    if (status != STATUS_SUCCESS) {
      return status;
    }
	}
	printf("\n"); 


	depth_first_traversal_core_and_send_data(next, 1, size_per_client, filefd, &offset /* , pointer to mesg_struct pre-filled with task id, group id */);

	return STATUS_SUCCESS;
}

server_error_codes_e depth_first_traversal_core_and_send_data( Node * t, int level, int size_per_client, int filefd, off_t *offset /* , pointer to mesg_struct pre-filled with task id, group id */)
{
	Node * left, * right;
  server_error_codes_e status;

	left = t -> leftchild;
	right = t -> rightchild;

  if ( left ) {
    if (  left-> bitpos > t->bitpos) 
    {
      printf("%*s",  level*2," ");
      printf("%d)Left Branch:bit %d %d\n",level ,
          left-> bitpos, left->node_count); 
      printf("\n"); 
      depth_first_traversal_core_and_send_data(left, level+1, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */);
    }
    else
    {
      printf("%*s",  level*2," ");
      printf("%d)Left:Data %#x %d\n", level, 
          (left->key&0xff000000u)>>24, 
          left->node_count); 
      status = read_and_send_data(left->key, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */);
      if (status != STATUS_SUCCESS) {
        return status;
      }
    }
  }
	printf("\n"); 
  if ( right ) {
    if (  right-> bitpos >t->bitpos) 
    {
      printf("%*s",  level*2," ");
      printf("%d)Right Branch:bit %d %d\n",level, 
          right-> bitpos ,right->node_count); 
      printf("\n"); 
      depth_first_traversal_core_and_send_data(right, level+1, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */);
    }
    else
    {
      printf("%*s",  level*2," ");
      printf("%d)Right:Data %#x %d\n",level, 
          (right->key&0xff000000)>>24, 
          right->node_count); 
      status = read_and_send_data(right->key, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */);
      if (status != STATUS_SUCCESS) {
        return status;
      }
    }
  }
  return STATUS_SUCCESS;
}

server_error_codes_e read_and_send_data (int sock_fd, int size_per_client, int filefd, off_t *offset /* , pointer to mesg_struct pre-filled with task id, group id */ )
{
  int n = 0;
  char buff[1000];
  int max_size = size_per_client;

  while (max_size >= 0) {
    n = pread (filefd, buff, 100 /* This size should be based on the array size of the message struct. */, *offset);
    if (n == -1) {
      return STATUS_FILE_READ_ERR;
    } else if (n == 0) {
      /* Update the Message type here. */
      break; /* This break has to be at different place after updating the message type */
    }

    /* Have to handle the space case */

    if((*offset = lseek(filefd, n ,SEEK_SET) < 0)) {
      return STATUS_CANT_SEEK;
    }

    /* send (sock_fd, msg_struct, sizeof(msg_struct), 0); */
    max_size -= n;
  }

  /* Close the file if end of file is reached. */

  return STATUS_SUCCESS;
}
