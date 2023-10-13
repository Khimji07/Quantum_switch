// Get Logs from switch using socket

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/var/run/roscmd.sock"

int main(int argc, char *argv[]) {

  if (argc <= 1) {
    printf("Invalid arguments\n");
  }

  int i = 0;
  char *cmd = malloc(50);
  for (i = 1; i < argc; i++) {
    strcat(cmd, argv[i]);
    if (i == (argc - 1)) {
      strcat(cmd, "\n");
    } else {
      strcat(cmd, " ");
    }
  }

  int sockfd;
  struct sockaddr_un server_address;
  char buffer[2048];
  char *prev_buf = "";
  char *curr_buf = malloc(20);

  // Create a socket
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(1);
  }

  // Set up the server address
  memset(&server_address, 0, sizeof(server_address));
  server_address.sun_family = AF_UNIX;
  strncpy(server_address.sun_path, SOCKET_PATH,
          sizeof(server_address.sun_path) - 1);

  // Connect to the socket
  if (connect(sockfd, (struct sockaddr *)&server_address,
              sizeof(server_address)) == -1) {
    perror("connect");
    exit(1);
  }

  // Send credentials
  write(sockfd, "admin\n", strlen("admin\n"));
  write(sockfd, "admin\n", strlen("admin\n"));

  write(sockfd, cmd, strlen(cmd));

  sleep(1);

  while (1) {
    ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
      perror("read");
      exit(1);
    }

    buffer[bytes_read] = '\0';
    printf("%s", buffer);
    sprintf(curr_buf, "%s", buffer);
    write(sockfd, " \n", strlen(" \n"));

    if (strlen(curr_buf) == strlen(prev_buf) && strchr(curr_buf, '#') &&
        strchr(prev_buf, '#')) {
      break;
    }

    prev_buf = curr_buf;
  }

  // Close the socket
  close(sockfd);

  return 0;
}
