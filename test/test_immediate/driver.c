//#include "../gdb-7.9/gdb/ME_common.c"
//#include "../driver/driver-interface-FE.c"

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "../ME_test.c"

void main(int argc, char *argv[]) {
  test_init();
  
  int sockfd = DI_init_measurer();
  char line[256];
  
  sprintf(line, "(set_target %s)",argv[1]);
  DI_send_request(sockfd, line);
  
  test_measurement(sockfd, "(measure (var \"a\"))", ME_measurement_create_string("45"));
  
  test_measurement(sockfd, "(measure (var \"b\"))", ME_measurement_create_string("2"));
  
  sprintf(line, "(gdb \"set test_stop=0\")");
  DI_send_request(sockfd, line);
    
  test_measurement(sockfd, "(measure (var \"a\"))", ME_measurement_create_string("46"));
  
  test_measurement(sockfd, "(measure (var \"b\"))", ME_measurement_create_string("2"));
  
  DI_send_request(sockfd, "(quit)");

  close(sockfd);
  
  test_close();  
}
