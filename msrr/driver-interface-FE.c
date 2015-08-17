/*#define DEBUG 1
#ifdef DEBUG
#define printd(...) { fprintf(stdout, __VA_ARGS__); }
#else
#define printd
#endif*/

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

int
DI_init_measurer (char * ip, int port)
{
  //printf("Launching GDB...\n");
  //system("/projects/zephyr/deadzone/ArmoredSoftware/llvm/gdb/mod1/gdb-7.9/gdb/gdb");

  printd("Connect socket... \n");
  int sockfd = 0, n = 0;
  char recvBuff[1024];
  struct sockaddr_in serv_addr;

  memset(recvBuff, '0',sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("\n Error : Could not create socket \n");
      return 1;
    }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  
  if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
  {
    printf("\n inet_pton error occured\n");
    return 1;
  }

  if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\n Error : Connect Failed \n");
    exit(-1);
    //return 1;
  }

  //fcntl(sockfd, F_SETFL, O_NONBLOCK);
  
  //printd("Connected to measurer %s !\n", li);

  return sockfd;
}

int request_id = 1;
ME_measurement * DI_send_request(int sockfd, char * request)
{
  if (strcmp(request,"(quit)")==0) {
    close(sockfd);
    exit(0);
  }

  //build JSON and send
  json_t *root, *result;
  json_error_t error;
  root = json_object();
  json_object_set_new(root, "jsonrpc", json_string("2.0"));
  json_object_set_new(root, "method", json_string("eval"));
  json_t *param_array = json_array();
  json_array_append(param_array,json_string(request));
  json_object_set_new(root, "params", param_array);   
  json_object_set_new(root, "id", json_integer(request_id));
  request_id++;
  
  char * send = json_dumps(root, 0);
  
  ME_sock_send(sockfd, send);

  json_decref(root);


  //get response
  char response[1024];
  int n = ME_sock_recv(sockfd, response);
  
  printd("Received:%s\n",response);

  root = json_loads(response, 0, &error);
  result = json_object_get(root,"result"); 

  //printf("Result = %s\n",json_string_value(result));

  ME_RLI_IR_value value = ME_RLI_IR_value_fromJSON(result);
  ME_RLI_IR_value_print(value);
//ME_measurement_print(result);
    

return NULL;//ME_measurement_fromJSON(result);

}

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

    DI_send_request(sockfd, line);
    
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
