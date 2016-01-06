#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <jansson.h>

#include <readline/readline.h>
#define clock history_clock
#include <readline/history.h>

#include <signal.h>

#include "../msrrd/gdb/ME_RLI_IR.h"

int request_id = 1;

int sockfd_copy;
void intHandler(int dummy) {
  printf("Caught SIGINT!\n");
  close(sockfd_copy);
  exit(-1);
}

void DI_interactive_mode(int sockfd)
{
  signal(SIGINT, intHandler);
  sockfd_copy = sockfd;
  
  rl_bind_key('\t',rl_abort); //disable auto-complete
  
  char* line=0;
  size_t line_buf_len=0;
  ssize_t curr_line_len;

  //set_conio_terminal_mode();
    
  fflush(stdout);
  
  while(true) {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    //FD_SET(0, &fds);
    FD_SET(sockfd, &fds);
        
    line = readline("\nmsrr> ");
    //curr_line_len=getline(&line, &line_buf_len, stdin);

    ME_sock_send_request(sockfd, line, request_id++);
    
    //fflush(stdout);
    if (line[0]!=0)
      add_history(line);

    /*int readsocks = select(sockfd+1, &fds, NULL, NULL, &tv);
    if (readsocks < 0) {
      printf("Select read error!\n");
      exit(-1);
    }	
    if (FD_ISSET(sockfd,&fds)) {
      DI_get_measurer_response(sockfd); 
      }*/
      
  }
}
