/* This program demonstrates a simple buffer overflow attack where the attacker can submit an incorrect password of sufficient length to overflow the 
 * control variable 'session' to initiate a session without knowing the password.
 * Recommended measurement: Measure both 'session' and 'password' to ensure the truth of this statement:
 *       session = 0 OR session==1 AND password = "12345"
 *    False implies a buffer overflow.
 */

#include <stdio.h>
#include <unistd.h>

int main() {
  char password[10];
  int session = 0;
  
  while (!session) {
    printf("\nEnter password: ");
    scanf("%s",password);

    if (strcmp(password,"12345")==0) {
      session = 1;
      printf("\nCorrect password! Initiating session...\n\n");
    } else {
      printf("\nIncorrect password! Try again.\n");
    }
  }

  char * buffer_input = NULL;
  size_t nbytes;

  printf("\n==========================================================\n");
  printf("Session started with priveleged_user.\n");
  printf("==========================================================\n"); 

  getline(&buffer_input,&nbytes,stdin);
  
  while (session) {
    printf("[priveleged_user@precious_machine]$ ");
    getline(&buffer_input,&nbytes,stdin);
    buffer_input[strlen(buffer_input)-1] = 0;
    printf("Executing dangerously powerful command '%s'...\n\n", buffer_input);
  }

  
}
