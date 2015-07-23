#include "../gdb-7.9/gdb/ME_common.c"
#include "../client/driver-interface-FE.c" 

FILE *f;
int test_assert_i=0;
int test_assert_passed=0;
int test_assert_failed=0;

void test_init() {
  char name;
  int  number;
  f = fopen("test.out", "w");
}
void test_close() {
  char message[64];
  sprintf(message,"\nran\t%d\npassed\t%d\nfailed\t%d\n",test_assert_i,test_assert_passed,test_assert_failed);
  log_write(message);
  fclose(f);
}
void log_write(char * text) {
  fprintf(f, "%s", text);
}
void test_assert(int value) {
  test_assert_i++;
  char message[64];
  if (value) {
    sprintf(message,"Assert %d: passed\n",test_assert_i);
    test_assert_passed++;
  }
  else
    {
      test_assert_failed++;
      sprintf(message,"Assert %d: failed\n",test_assert_i);
    }
  log_write(message);
}

void test_measurement(int sockfd, char * request, ME_measurement * mse) {
  ME_measurement * msa = DI_send_request(sockfd, request);
  test_assert(ME_measurement_equal(msa,mse));
}

void test_continue(int sockfd) {
  DI_send_request(sockfd, "(gdb \"set test_stop=0\")"); 
}
