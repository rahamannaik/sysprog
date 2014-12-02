#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pthread_t thread_id;
    pthread_attr_t attr;


    char buffer[256];

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
    if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) 
    {
         perror("ERROR connecting");
         exit(1);
    }	

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&thread_id, &attr, join_group, NULL); 

    /* Now ask for a message from the user, this message
    * will be read by server
    */
    //printf("Please enter the message: ");
    bzero(buffer,256);
    
    //setbuf(stdout, NULL);
    //setvbuf (stdout, NULL, _IONBF, BUFSIZ);
    while(1)
    {
     unsigned int option;
     //setvbuf (stdout, NULL, _IONBF, BUFSIZ);
     printf("select 1 to join a group\n");
     //fflush(stdout);
     printf("select 2 to send message to a group\n");
     //fflush(stdout);
     printf("Enter the option: ");
     //fflush(stdout);

     //scanf("%d",&option);
     //fgets(buffer,2,stdin);
     //option = atoi(buffer);
     //sscanf(buffer,"%d", &option);
     //fflush(stdin);

     //setvbuf (stdin, NULL, _IONBF, BUFSIZ);
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
        //printf("Please enter valid option\n");
        continue;
       }
     
     /*if(option !=1 || option!=2)
     {
      printf("invalid option\n");
      continue;
     }*/

     //setvbuf (stdin, NULL, _IONBF, BUFSIZ);

     switch(option)
     {
      case 1:
       enter_groupid:
       printf("Please enter the groupid \n");
       if(fgets(buffer,255,stdin))
       {
        //setvbuf (stdin, NULL, _IONBF, BUFSIZ);
        if(sscanf(buffer,"%d", &option) == 1)
        {
         snprintf(buffer, 15,"join groupid %d", option);
         //setvbuf (stdin, NULL, _IONBF, BUFSIZ);
        }
        else
        {
         printf("\nInvalid groupid\n");
         goto enter_groupid;
         //continue;
        }
       }
       else
       continue;
       //scanf("%d", &option);
       //snprintf(buffer, 15,"join groupid %d", option);
       //setvbuf (stdin, NULL, _IONBF, BUFSIZ);
      break;
      
      case 2:
       printf("Please enter the message: ");
       fgets(buffer,255,stdin);
       //setvbuf (stdin, NULL, _IONBF, BUFSIZ);
      break;
     
      default:
      printf("\ninvalid option\n");
      continue;
     }

     //fgets(buffer,255,stdin);
    
     /* Send message to the server */
      //bzero(buffer,256);
     n = write(sockfd,buffer,strlen(buffer));
     if (n < 0) 
     {
         perror("ERROR writing to socket");
         exit(1);
     }
      bzero(buffer,256);
      /* Now read server response */
      n = read(sockfd,buffer,255);
      if (n < 0) 
      {
           perror("ERROR reading from socket");
           exit(1);
      }
      printf("%s\n",buffer);
      bzero(buffer,256);
    }
   
    return 0;
}

