//#include "../gdb-7.9/gdb/ME_common.c"
//#include "../driver/driver-interface-FE.c"

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "../ME_test.c"

void main(int argc, char *argv[]) {
  test_init();

  int port = atoi(argv[1]);
  
  int sockfd = DI_init_measurer(port);
  char line[256];
  
  sprintf(line, "(set_target %s)",argv[2]);
  DI_send_request(sockfd, line);

  test_start(sockfd);
  
  test_measurement(sockfd, "(measure (var \"a\"))", ME_measurement_create_string("45"));
  
  test_measurement(sockfd, "(measure (var \"b\"))", ME_measurement_create_string("2"));

  test_continue(sockfd);
  
  test_measurement(sockfd, "(measure (var \"a\"))", ME_measurement_create_string("46"));
  
  test_measurement(sockfd, "(measure (var \"b\"))", ME_measurement_create_string("2"));
  
  DI_send_request(sockfd, "(quit)");

  close(sockfd);
  
  test_close();  
}
