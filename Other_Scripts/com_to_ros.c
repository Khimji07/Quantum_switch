#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/var/run/roscmd.sock"

int main() {
  int sockfd;
  struct sockaddr_un server_address;
  char buffer[256];

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

  // Read the login prompt
  ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  if (bytes_read == -1) {
    perror("read");
    exit(1);
  }
  buffer[bytes_read] = '\0';
  printf("%s", buffer);

  // Enter credentials
  char username[256];
  char password[256];
  printf("Username: ");
  fgets(username, sizeof(username), stdin);
  printf("Password: ");
  fgets(password, sizeof(password), stdin);

  // Send credentials
  write(sockfd, username, strlen(username));
  write(sockfd, password, strlen(password));

  // Receive response
  bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  if (bytes_read == -1) {
    perror("read");
    exit(1);
  }
  buffer[bytes_read] = '\0';
  printf("%s", buffer);

  sleep(2);
  write(sockfd, "do terminal no prompt\n", strlen("do terminal no prompt\n"));
  write(sockfd, "config\n", strlen("config\n"));
  write(sockfd, "vlan database\n", strlen("vlan database\n"));
  write(sockfd, "vlan 100\n\n", strlen("vlan 100\n\n"));

  sleep(2);

  // Close the socket
  close(sockfd);

  return 0;
}
