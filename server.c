/*******select.c*********/

/*******Using select() for I/O multiplexing */


//#include "server_patricia.h"
#include "server.h"
#include "common.h"

/* port we're listening on */

#define PORT 2020

 
grp_info group_info[1000]; 


int main(int argc, char *argv[])
{

  /* server address */

  struct sockaddr_in serveraddr;

  /* client address */

  struct sockaddr_in clientaddr;

  /* master file descriptor list */

  fd_set master;

  /* temp file descriptor list for select() */

  fd_set read_fds;

  /* maximum file descriptor number */

  int fdmax;

  /* listening socket descriptor */

  int listener;

  /* newly accept()ed socket descriptor */

  int newfd;

  int nbytes;

  /* for setsockopt() SO_REUSEADDR, below */

  int yes = 1;

  int addrlen;

  int i;

  /* clear the master and temp sets */

  FD_ZERO(&master);

  FD_ZERO(&read_fds);

  struct sigaction sigact;

  sigact.sa_handler = &handler_display_result;

  sigact.sa_flags = SA_RESTART;

  /* Block every signal during the handler */
  sigfillset(&sigact.sa_mask);

  /* Intercept SIGALRM */

  if(sigaction(SIGALRM, &sigact, NULL) == -1) {
    perror("Error: cannot handle SIGALRM");
  }

  /* get the listener */

  if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)

  {

    perror("Server-socket() error !");

    /*just exit !*/

    exit(1);

  }

  printf("Server-socket() is OK...\n");

  /*"address already in use" error message */

  if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)

  {

    perror("Server-setsockopt() error !");

    exit(1);

  }

  printf("Server-setsockopt() is OK...\n");



  /* bind */

  serveraddr.sin_family = AF_INET;

  serveraddr.sin_addr.s_addr = INADDR_ANY;

  serveraddr.sin_port = htons(PORT);

  memset(&(serveraddr.sin_zero), '\0', 8);



  if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)

  {

    perror("Server-bind() error !");

    exit(1);

  }

  printf("Server-bind() is OK...\n");



  /* listen */

  if(listen(listener, 10) == -1)

  {

    perror("Server-listen() error !");

    exit(1);

  }

  printf("Server-listen() is OK...\n");



  /* add the listener to the master set */

  FD_SET(listener, &master);

  /* keep track of the biggest file descriptor */

  fdmax = listener; /* so far, it's this one*/


  int temp = 0;

  /* loop */

  for(;;)

  {


    /* copy it */

    read_fds = master;

    //printf("updating socket fds\n");


    if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)

    {

      perror("Server-select() error !");

      exit(1);

    }

    temp++;
    printf("Server-select() is OK for %d time ...\n", temp);



    /*run through the existing connections looking for data to be read*/

    for(i = 0; i <= fdmax; i++)

    {

      if(FD_ISSET(i, &read_fds))

      { /* we got one... */

        if(i == listener)

        {

          /* handle new connections */

          addrlen = sizeof(clientaddr);

          if((newfd = accept(listener, (struct sockaddr *)&clientaddr, (socklen_t *) &addrlen)) == -1)

          {

            perror("Server-accept() error !");

          }

          else

          {

            printf("%s:%d, Server-accept() is OK... New sock fd :%d\n", __func__, __LINE__, newfd);



            FD_SET(newfd, &master); /* add to master set */

            if(newfd > fdmax)

            { /* keep track of the maximum */

              fdmax = newfd;

            }

            printf("newsockfd is set\n");
            //printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), newfd);

            /*read_fds = master;

              if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)

              {

              perror("Server-select() error !");

              exit(1);

              }

              printf("Server-select() is OK...\n");*/

          }
        }

        else

        {

          printf("handle data from a client\n");
          /* handle data from a client */

          u_char msg_type;
          if((nbytes = recv(i, &msg_type, sizeof(msg_type), 0)) <= 0)
            //if((nbytes = recvall(i, &msg_type, sizeof(msg_type))) <= 0)
            //if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)

          {

            if(nbytes == 0)

              /* connection closed */

              printf("%s: socket %d hung up\n", argv[0], i);



            else

              perror("recv() error !");

            //delete_patricia(groupinfo[group_id-1].root, i);


            /* close it... */

            close(i);

            /* remove from master set */

            FD_CLR(i, &master);

          }

          else

          {

            printf("Message received is :%c\n",msg_type);

            u_char task_id;
            u_short group_id;
            u_short cnt = 0;
            u_short data_len;

            if(recvall(i, &task_id, sizeof(task_id)) < 0)
            {
              perror("ERROR reading from socket");
              exit(1);
            }

            if(recvall(i, (char *)&group_id, sizeof(group_id)) < 0)
            {
              perror("ERROR reading from socket");
              exit(1);
            }
            group_id = ntohs(group_id);
            printf("%s:%d, groud id : %d\n", __func__, __LINE__, group_id);
            printf("%s:%d, Message Type : %d\n", __func__, __LINE__, msg_type);

            if(recvall(i, (char *)&data_len, sizeof(data_len)) < 0)
            {
              perror("ERROR reading from socket");
              exit(1);
            }

            if(JOIN_GROUP == msg_type)
            {
              int status = insert(&group_info[group_id-1].root, (DataType)i);
              if(status == TRIE_INSERT_SUCCESS)
              {
                group_info[group_id-1].clnt_count++;
                printf("Insertion successful in group %d, count is %d\n", group_id, group_info[group_id-1].clnt_count);

                cnt++;
                if(cnt == 1)
                {
                  printf("First client request\n");

                  pthread_t thread_id1;
                  pthread_attr_t attr;

                  pthread_attr_init(&attr);
                  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

                  pthread_create(&thread_id1, &attr, handle_user_input, NULL);
                
                }
              }
              else
              {
                printf("Error during client joining\n");
              }
            }
            else if(LEAVE_GROUP == msg_type)
            {
              printf("Delete req received\n");
              delete(&group_info[group_id-1].root, (DataType)i);
              //delete_patricia(groupinfo[group_id-1].root, i);
              group_info[group_id-1].clnt_count--;
            }
            else if(REPLY_FROM_CLIENT == msg_type)
            {
              printf("%s:%d, Received Reply from Client : %d\n", __func__, __LINE__, i);
   /*           u_short data_len;
              if(recvall(i, (char *)&data_len, sizeof(data_len)) < 0)
              {
                perror("ERROR reading from socket");
                exit(1);
              } */
              data_len = ntohs(data_len);
              printf("\n Data len = %d", data_len);

              u_int data;

              if(recvall(i, (char *)&data, data_len) < 0)
              {
                perror("ERROR reading from socket");
                exit(1);
              }

              data = ntohl(data);

              char filename[256], output[sizeof(long)];
              size_t nread = 0;
              long result = 0;
              FILE *file;


              snprintf(filename, sizeof(filename), "group%d_task%d.txt", group_id, task_id);

              // here we get some data into variable data

              file = fopen(filename, "r+");
              if(!file)
              {
                file = fopen(filename, "w+");
                if(!file)
                { 
                  printf("Error in opening file\n");
                  exit(1);
                } 
              }

              printf("Opening file successful\n");

              //while ((nread = fread(output, sizeof(long), 1, file)) > 0);
              nread = fread(output, sizeof output, 1, file);

              printf("out of file read, nread = %d\n", (int)nread);

              if(nread)
                result = atol(output);

              printf("result after reading from file %lu\n",result);

              if(task_id == (unsigned short int)1)
              {
                if(data > result)
                  result = data;
              }
              else
              {
                result += data;
              }

              printf("Final result  %lu",result);
              snprintf(output, sizeof output, "%lu", result);

              fseek( file, 0, SEEK_SET );
              //while (fwrite(output,1, sizeof(long),file) > 0);
              fwrite(output, sizeof(long), 1, file);

              fclose(file);

            }
            else
            {
              printf("Unknown data\n");
            }

          }
        }

      }

    }

  }
  pthread_exit(NULL);
  return 0;
}


void *handle_user_input(void *dummy)
{
  char buffer[256];
  int file;

  bzero(buffer,256);

  unsigned short taskid = 0 ;
  unsigned short groupid = 0;

  while(1)
  {
    printf("select 1 for max of numbers task\n");
    printf("select 2 for word count task\n");
    printf("Enter taskid:");

    //scanf("%d", &taskid);
    if(fgets(buffer,255,stdin))
    {
      if(sscanf(buffer,"%hu", &taskid) != 1)
      {
        printf("Please enter valid taskid\n");
        continue;
      }
    }
    else
    {  
      printf("fgets failed for taskid\n");
      continue;
    }

    /*if(taskid != (unsigned short)1 || taskid!=(unsigned short)2)
      {
      printf("invalid taskid\n");
      continue;
      }*/

    switch(taskid)
    {
      case 1:
      case 2:
        break;
      default:
        printf("invalid taskid\n");
        continue;
    }

enter_groupid:

    //scanf("%d", &groupid);
    bzero(buffer,256);
    printf("Please enter the groupid \n");
    if(fgets(buffer,255,stdin))
    {
      if(sscanf(buffer,"%hu", &groupid) == 1)
      {
        //snprintf(buffer, 15,"join groupid %d", groupid);
      }
      else
      {
        printf("\nInvalid groupid\n");
        goto enter_groupid;
        //continue;
      }
    }
    else
    {
      printf("fgets failed for groupid\n");
      continue;
    }
enter_filename:

    printf("Please enter the filename \n");
    fgets(buffer,255,stdin);

    char *pos;
    if ((pos=strchr(buffer, '\n')) != NULL)
      *pos = '\0';

    file = open(buffer, O_RDONLY);

    if(!file)
    {
      printf("Error in opening file. Enter existing file\n"); 
      goto enter_filename;
    }

    struct stat st;
    stat(buffer, &st);
    long file_size = st.st_size;

    handle_task(taskid, groupid, file, file_size);
    close(file);
  }
  return NULL;
}

void handle_task(unsigned short taskid, unsigned short groupid, int file, long file_size)
{
 int size_per_client =0;
 off_t offset = 0;
 server_error_codes_e status;

 printf("Handling task\n");

 printf("file_size =%lu client count in group %d is %d\n", file_size, groupid, group_info[groupid-1].clnt_count);

 size_per_client = (file_size/group_info[groupid-1].clnt_count);

 status = depth_first_traversal_and_send_data(group_info[groupid-1].root, size_per_client, file, &offset, taskid, groupid);
 
 if(STATUS_SUCCESS == status) {

   /* Copying the taskid and groupid into the global variables for the signal handler */
   sig_taskid = taskid;

   sig_groupid = groupid;

   /* Wait for 1 second to get response from all the clients
    * and the result would be displayed in handler function */
   alarm(1);
 }
}

server_error_codes_e depth_first_traversal_and_send_data(Node * t, int size_per_client, int filefd, off_t *offset, u_short taskid, u_short groupid)
{
  if ( !t) return STATUS_NO_CLI_IN_GRP; // root is null

  printf("Depth traversal\n");

  Node * current, *next;
  //filefd = open ("./Random_Numbers.txt", O_RDONLY);
  current = t;
  next = t-> leftchild;
  if ( next-> bitpos > t->bitpos)
  {
    printf("0)Left Branch:bit %d %d\n",
        next-> bitpos, next->node_count);
  }
  else
  {
    printf("0)Left:Data %#x %d\n",
        (next->key),
        next->node_count);
    
    /*status = read_and_send_data(next->key, size_per_client, filefd, offset, taskid, groupid);
    //status = basic_read_and_send_data(next->key, size_per_client, filefd, offset, taskid, groupid);
    if (status != STATUS_SUCCESS) {
      return status;
    } */
  }
  printf("\n");
  //depth_first_traversal_core_and_send_data(next, 1, size_per_client, filefd, &offset /* , pointer to mesg_struct pre-filled with task id, group id */, taskid, groupid);
	depth_first_traversal_core_and_send_data(next, 1, size_per_client, filefd, offset, taskid, groupid/* , pointer to mesg_struct pre-filled with task id, group id */);
  return STATUS_SUCCESS;
}

server_error_codes_e depth_first_traversal_core_and_send_data( Node * t, int level, int size_per_client, int filefd, off_t *offset, u_short taskid, u_short groupid)
{
  Node * left, * right;
  server_error_codes_e status;
  left = t -> leftchild;
  right = t -> rightchild;
  if ( left ) {
    if ( left-> bitpos > t->bitpos)
    {
      printf("%*s", level*2," ");
      printf("%d)Left Branch:bit %d %d\n",level ,
          left-> bitpos, left->node_count);
      printf("\n");
      depth_first_traversal_core_and_send_data(left, level+1, size_per_client, filefd, offset, taskid, groupid /* , pointer to mesg_struct pre-filled with task id, group id */);
      //depth_first_traversal_core_and_send_data(left, level+1, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */, taskid, groupid);
    }
    else
    {
      printf("%*s", level*2," ");
      printf("%d)Left:Data %#x %d\n", level,
          (left->key),
          left->node_count);
      status = read_and_send_data(left->key, size_per_client, filefd, offset, /* , pointer to mesg_struct pre-filled with task id, group id */taskid, groupid);
      //status = basic_read_and_send_data(left->key, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */, taskid, groupid);
      if (status != STATUS_SUCCESS) {
        return status;
      }
    }
  }
  printf("\n");
  if ( right ) {
    if ( right-> bitpos >t->bitpos)
    {
      printf("%*s", level*2," ");
      printf("%d)Right Branch:bit %d %d\n",level,
          right-> bitpos ,right->node_count);
      printf("\n");
      depth_first_traversal_core_and_send_data(right, level+1, size_per_client, filefd, offset, taskid, groupid /* , pointer to mesg_struct pre-filled with task id, group id */);
      //depth_first_traversal_core_and_send_data(right, level+1, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */, taskid, groupid);
    }
    else
    {
      printf("%*s", level*2," ");
      printf("%d)Right:Data %#x %d\n",level,
          (right->key),
          right->node_count);
      status = read_and_send_data(right->key, size_per_client, filefd, offset,  /* , pointer to mesg_struct pre-filled with task id, group id */ taskid, groupid);
      //status = basic_read_and_send_data(right->key, size_per_client, filefd, offset /* , pointer to mesg_struct pre-filled with task id, group id */, taskid, groupid);
      if (status != STATUS_SUCCESS) {
        return status;
      }
    }
  }
  return STATUS_SUCCESS;
}

server_error_codes_e basic_read_and_send_data (unsigned int sock_fd, int size_per_client, int filefd, off_t *offset, u_short task_id, u_short group_id)
{
  long msg_len  = sizeof(message) + size_per_client;
  message *ptr = malloc(msg_len);

  ptr->msg_type = START_OF_TASK;
  ptr->task_id = task_id;
  ptr->group_id = htons(group_id);
  ptr->data_len = htons(msg_len);

  printf("%s:%d, Size Per Client :%d\n", __func__, __LINE__, size_per_client);
  printf("%s:%d, Task Id :%d\n", __func__, __LINE__, ptr->task_id);
  printf("%s:%d, htons'd Group Id :%d, Actual Group Id :%d\n", __func__, __LINE__, ptr->group_id, group_id);
  printf("%s:%d, htons'd Data Len :%d, Actual Data Len :%ld\n", __func__, __LINE__, ptr->data_len, msg_len);
  //while(fread(ptr->data, 1, size_per_client, file));
  read(filefd, ptr->data, size_per_client);
  //printf("Data That I am gonna send : %s\n", ptr->data);
  //fread(ptr->data, 1, size_per_client, (FILE *)filefd);

  *offset = lseek(filefd, size_per_client, SEEK_SET);

  if(sendall(sock_fd, (char *)ptr, msg_len) == -1)
  {
    perror("ERROR writing to socket");
    exit(1);
  }

  free(ptr);
  /* Close the file if end of file is reached. */
  return STATUS_SUCCESS;
}

server_error_codes_e read_and_send_data (int sock_fd, int size_per_client, int filefd, off_t *offset, u_short task_id, u_short group_id)
{
  int n = 0;
  //char buff[1000];
  char c = '\0';
  int still_to_read = 0;
  int max_size = size_per_client;
  long msg_len  = sizeof(message) + size_per_client;
  message *ptr = malloc(msg_len); 
  static unsigned char is_start_task = TRUE;

  if (is_start_task) {
    ptr->msg_type = START_OF_TASK;
    is_start_task = FALSE;
  } else {
    ptr->msg_type = TASK_DATA;
  }
  ptr->task_id = task_id;
  ptr->group_id = htons(group_id);

  printf("%s:%d, Size Per Client :%d\n", __func__, __LINE__, size_per_client);
  printf("%s:%d, Task Id :%d\n", __func__, __LINE__, ptr->task_id);
  printf("%s:%d, htons'd Group Id :%d, Actual Group Id :%d\n", __func__, __LINE__, ptr->group_id, group_id);
  printf("%s:%d, Message Type :%d\n", __func__, __LINE__, ptr->msg_type);

  while (max_size > 0) {
    n = pread (filefd, ptr->data, size_per_client, *offset);
    if (n == -1) {
      return STATUS_FILE_READ_ERR;
    } else if (n == 0) {
      ptr->msg_type = END_OF_TASK;
      /* Update the Message type here. */
      ptr->data[n + still_to_read] = '\0';
      ptr->data_len = htons(msg_len + still_to_read);
      printf("%s:%d, htons'd Data Len :%d, Actual Data Len :%ld\n", __func__, __LINE__, ptr->data_len, (msg_len + still_to_read));
      printf("%s:%d, Number of bytes read : %d\n", __func__, __LINE__, n);
      /* send (sock_fd, msg_struct, sizeof(msg_struct), 0); */
      if(sendall(sock_fd, (char *)ptr, (msg_len + still_to_read)) == -1)
      {
        perror("ERROR writing to socket");
        exit(1);
      }

      break; /* This break has to be at different place after updating the message type */
    }

    /* TODO :: Move this if block, after this while loop if you are NOT reading 
     * the entire size in one go. 
     */
    /*if (task_id == 1) {
      message *temp_var = NULL;
      if(ptr->data[n] != ' ') {
        FILE* fp = fdopen(filefd, "r");
        if((*offset = lseek(filefd, n, (SEEK_SET))) < 0) {
          return STATUS_CANT_SEEK;
        }

        if (fseek(fp, n, (SEEK_SET)) < 0) {
          return STATUS_CANT_SEEK;
        }

        while (c != EOF) {
          fread(&c, 1, 1, fp);
          if (c == ' ') {
            still_to_read++;
            break;
          }
        }

        temp_var = (message*) malloc (sizeof(message) + n + still_to_read + 1);
        assert(temp_var);
        memcpy(temp_var, ptr, (sizeof(message) + n));
        free(ptr);
        ptr = temp_var;
        temp_var = NULL;

        if(pread(filefd, &(ptr->data[n]), still_to_read, *offset) != still_to_read) {
          return STATUS_FILE_READ_ERR;
        }
      }
    } */
    ptr->data[n + still_to_read] = '\0';

    /* Have to handle the space case */

    if((*offset = lseek(filefd, (n + still_to_read) ,SEEK_SET) < 0)) {
      return STATUS_CANT_SEEK;
    }

    ptr->data_len = htons(msg_len + still_to_read);
    printf("%s:%d, htons'd Data Len :%d, Actual Data Len :%ld\n", __func__, __LINE__, ptr->data_len, (msg_len + still_to_read));
    printf("%s:%d, Number of bytes read : %d\n", __func__, __LINE__, n);
    if(send (sock_fd, ptr, (msg_len + still_to_read), 0) == -1) 
//    if(sendall(sock_fd, ptr, (msg_len + still_to_read)) == -1)
    {
      perror("ERROR writing to socket");
      exit(1);
    }
    max_size -= n;
    printf("%s:%d, max_size: %d\n", __func__, __LINE__, max_size);
  }

  /* Close the file if end of file is reached. */
  //ptr->data[0] = '\0';
  memset(ptr->data, 0, (size_per_client));
  ptr->msg_type = END_OF_TASK;
  ptr->data_len = sizeof(message);
  if(send (sock_fd, ptr, sizeof(message), 0) == -1) 
  //if(sendall(sock_fd, ptr, sizeof(message)) == -1)
  {
    perror("ERROR writing to socket");
    exit(1);
  }

  return STATUS_SUCCESS;
}

void handler_display_result(int signal)
{
  char resultfile[256], output[sizeof(long)];
  size_t nread = 0;
  long result = 0;
  FILE *file;

  if (signal == SIGALRM) {
    //printf("Got SIGALRM\n");

    snprintf(resultfile, sizeof(resultfile), "group%d_task%d.txt", sig_groupid, sig_taskid);

    file = fopen(resultfile, "r");

    if(!file)
    {
      printf("Error in opening resultfile\n");
      exit(1);
    }
    printf("Opening resultfile successful\n");

    nread = fread(output, sizeof(output), 1, file);

    if(nread) {
      result = atol(output);
    }

    printf("**** CONSOLIDATED RESULT FOR TASK %d ****\n", sig_taskid);

    if(sig_taskid == MAX_NUM) {
      printf("The Maximum number in the Given file is %lu\n", result);
    }
    else
    {
      printf("The Number of words in the Given file is %lu\n", result);
    }

    fclose(file);
  }

  printf("Caught wrong signal: %d\n", signal);
}


