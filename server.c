#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct group_member
{
 struct sockaddr_in client_addr;
 int client_port;
 //char buffer[1000];
 struct group_member *next;
};

typedef struct group_member group_member_t;

group_member_t *insert_group_list(int groupid, struct sockaddr_in *client_addr);

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
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    unsigned long pid, pid1;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
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
                (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
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
        printf("groupid is zero. Server exiting...\n");
        exit(0);
       }
       grpmember = insert_group_list(groupid, client_addr);     
       
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
