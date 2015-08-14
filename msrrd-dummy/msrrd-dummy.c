#include "../gdb-7.9/gdb/ME_common.c"

int main(int arc, char **argv)  {
  char * portarg = argv[1];

  if (!portarg) {
    printf("Failed to specify port as first argument!\n");
    exit(-1);
  }
  int port = atoi(portarg);

  int driverfd = ME_sock_server_connect(port);
   
  while (1) {
    char request[1024];
    int n = ME_sock_recv(driverfd, request);
    
    if (n <= 0 || !(*request))
      continue;

    //Send response
    char response[] = "{\"jsonrpc\": \"2.0\", \"result\": {\"next\": null, \"type\": \"string\", \"data\": \"0\"}, \"id\": 6}";
    
    ME_sock_send(driverfd, response);  

    if (strstr(request,"(quit)")!=NULL) {
      printf("\Quiting...\n");
      close(driverfd);
      exit(-1);
    }

  }
}
