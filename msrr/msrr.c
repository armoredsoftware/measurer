#include "../msrrd/gdb/ME_common.c"
#include "driver-interface-FE.c"

int main(int arc, char **argv)  {
  char * iparg = argv[1];  //TODO - proper input validation
  char * portarg = argv[2];
  
  if (!portarg) {
    printf("Failed to specify port as first argument!\n");
    exit(-1);
  }
  
  int port = atoi(portarg);
  
  int sockfd = DI_init_measurer(iparg,port);

  DI_interactive_mode(sockfd); 
  
}
