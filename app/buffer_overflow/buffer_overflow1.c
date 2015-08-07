/* This application demonstrates a simple buffer overflow attack. 'string_invariant' should never change but will if the user provides a
 * sufficiently long input.
 * Recommended measurement: Measure string_invariant during the loop and check the result to match "EXPECTED_VALUE"; anything other result implies overflow.
 */

#include <stdio.h>
#include <unistd.h>

int main() {
  char string_invariant[] = "EXPECTED_VALUE";
  char buffer_input[10];
  
  while (1) {    
    printf("Enter a value:");
    scanf("%s",buffer_input);    
    printf("string_invariant = \"%s\"\nbuffer_input = \"%s\"\n",string_invariant,buffer_input);    
  }
}
