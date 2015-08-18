#include "../msrrd/gdb/ME_common.h"
#include "../msrrd/gdb/ME_RLI_IR.h"
#include "driver-interface-FE.c"

int main(int argc, char **argv)  {
  int dflag = 0;
  int pflag = 0;
  char * pvalue = NULL;
  int index;
  int c;
  static char usage[] = "usage: msrr [-d] [-p port] hostname \n";
  static int default_port = 3000;
  
  opterr = 0;
  while ((c = getopt (argc, argv, "dp:")) != -1)
    switch (c) {
    case 'd':
      dflag = 1;
      break;
    case 'p':
      pflag = 1;
      pvalue = optarg;
      break;
    case '?':
      if (optopt == 'p') {
	fprintf(stderr, "option requires an argument -- %c\n", optopt);
      } else if (isprint (optopt)) {
	  fprintf (stderr, "unknown option -- %c\n", optopt);
      } else {
	  fprintf (stderr, "unknown option character `\\x%x'.\n", optopt);
      }
      fprintf(stderr, usage);
      return 1;
    default:
      abort();
    }

  char * address = NULL;
    
  for (index = optind; index < argc; index++) {
    address = argv[index];
    break;
  }
  
  if (!address) {
    fprintf(stderr, "failed to specify hostname\n");
    fprintf(stderr, usage);
    return 1;
  }
  
  int port = (pvalue ? atoi(pvalue) : default_port);
  if (!port) {
    fprintf(stderr,"option requires integer argument -- p\n");
    fprintf(stderr, usage);
    return 1;
  }

  ME_DEBUG = dflag;
  
  int sockfd = DI_init_measurer(address,port);

  DI_interactive_mode(sockfd); 
  
}
