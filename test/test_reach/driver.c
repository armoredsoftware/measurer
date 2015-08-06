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

  DI_send_request(sockfd, "(hook (reach \"app.c\" 11 0) '(store 1 (measure (var \"a\"))))");
  DI_send_request(sockfd, "(hook (reach \"app.c\" 13 0) '(store 2 (measure (var \"a\"))))");
  
  sprintf(line, "(gdb \"set start_stop=0\")",argv[2]);
  DI_send_request(sockfd, line);

  sleep(1);

  test_measurement(sockfd, "(load 1)", ME_measurement_create_string("5"));
  
  test_measurement(sockfd, "(load 2)", ME_measurement_create_string("7"));
  
  DI_send_request(sockfd, "(quit)");

  test_close();  
}
