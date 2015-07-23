#include "../gdb-7.9/gdb/ME_common.c"
#include "driver-interface-FE.c"

int main(int arc, char **argv)  {
  char * portarg = argv[1];

  if (!portarg) {
    printf("Failed to specify port as first argument!\n");
    exit(-1);
  }
  int port = atoi(portarg);
  
  int sockfd = DI_init_measurer(port);

  DI_interactive_mode(sockfd);
 
  
}
