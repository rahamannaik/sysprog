#include "common.h"

int sendall(int s, char *buf, int len)                                     
{                                                                          
  int total = 0;        // how many bytes we've sent                       
  int bytesleft = len; // how many we have left to send                    
  int n;                                                                   


//  printf("%s:%d, Socket fd : %d\n", __func__, __LINE__, s);
  while(total < len)                                                       
  {                                                                        
    n = send(s, buf+total, bytesleft, 0);                                  
    if (n == -1)                                                           
    {                                                                      
      break;                                                               
    }                                                                      
    total += n;                                                            
    bytesleft -= n;                                                        
  }                                                                        

  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success           
}                                                                          

int recvall(int s, char *buf, int len)                                     
{                                                                          
  int total = 0;        // how many bytes we've received                   
  int bytesleft = len; // how many we have left to received                
  int n;                                                                   

  while(total < len)                                                       
  {                                                                        
    n = recv(s, buf+total, bytesleft, MSG_WAITALL);                                     
    if (n == -1)                                                           
    {                                                                      
      break;                                                               
    }                                                                      
    total += n;                                                            
    bytesleft -= n;                                                        
  }
//  buf[total] = '\0';

  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success           

}


