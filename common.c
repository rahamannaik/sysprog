#include "common.h"

int sendall(int s, char *buf, int len)                                     
{                                                                          
  int total = 0;        // how many bytes we've sent                       
  int bytesleft = len; // how many we have left to send                    
  int n;                                                                   

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

int readall(int s, char *buf, int len)                                     
{                                                                          
  int total = 0;        // how many bytes we've received                   
  int bytesleft = len; // how many we have left to received                
  int n;                                                                   

  while(total < len)                                                       
  {                                                                        
    n = read(s, buf+total, bytesleft);                                     
    if (n == -1)                                                           
    {                                                                      
      break;                                                               
    }                                                                      
    total += n;                                                            
    bytesleft -= n;                                                        
  }                                                                        

  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success           

}


