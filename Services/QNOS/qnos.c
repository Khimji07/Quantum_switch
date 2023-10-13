#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define SOCKET_PATH "/var/run/roscmd.sock"
#define CMD_DIR "/tmp/qnos"
#define MAX_LINE_LENGTH 256
int sockfd;
char buffer[1024];
char *linesArray[500]; // Array to store the lines
int lineCount = 0;    // Counter for the number of linesArray
char *filesArray[50];
int filecount = 0;

typedef struct {
  char name[256];
  time_t timestamp;
} FileEntry;

int compareFileEntries(const void *a, const void *b) {
  const FileEntry *fileA = (const FileEntry *)a;
  const FileEntry *fileB = (const FileEntry *)b;

  if (fileA->timestamp < fileB->timestamp) {
    return -1;
  } else if (fileA->timestamp > fileB->timestamp) {
    return 1;
  } else {
    return 0;
  }
}

int get_files() {
  DIR *dir;
  struct dirent *entry;
  struct stat fileStat;
  char path[256];
  FileEntry *fileEntries = NULL;

  dir = opendir(CMD_DIR);
  if (dir == NULL) {
    perror("opendir");
    return EXIT_FAILURE;
  }

  while ((entry = readdir(dir)) != NULL) {
    snprintf(path, sizeof(path), "%s/%s", CMD_DIR, entry->d_name);

    if (stat(path, &fileStat) < 0) {
      perror("stat");
      return EXIT_FAILURE;
    }

    if (S_ISREG(fileStat.st_mode)) {
      fileEntries = realloc(fileEntries, (filecount + 1) * sizeof(FileEntry));
      strcpy(fileEntries[filecount].name, entry->d_name);
      fileEntries[filecount].timestamp = fileStat.st_mtime;

      filecount++;
    }
  }

  qsort(fileEntries, filecount, sizeof(FileEntry), compareFileEntries);

  int i = 0;
  char *file = malloc(50);
  for (i = 0; i < filecount; i++) {
    sprintf(file, "%s/%s", CMD_DIR, fileEntries[i].name);
    filesArray[i] = strdup(file);
  }

  free(file);
  free(fileEntries);
  closedir(dir);

  return EXIT_SUCCESS;
}

int read_file(char *filename) {
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), fp)) {
    if (!strcmp(line, "\n")) {
      continue;
    }
    // Allocate memory for the line and copy it
    char *storedLine = strdup(line);

    // Store the line in the array
    linesArray[lineCount] = storedLine;
    lineCount++;
  }

  strcat(linesArray[lineCount - 1], "\n");

  // Access and print the stored lines
  fclose(fp);
  return EXIT_SUCCESS;
}

int connect_socket() {
  struct sockaddr_un server_address;

  // Create a socket
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    return EXIT_FAILURE;
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
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int login_shell() {
  // Read the login prompt
  ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  if (bytes_read == -1) {
    perror("read");
    return EXIT_FAILURE;
  }
  buffer[bytes_read] = '\0';
  printf("%s", buffer);

  // Send credentials
  write(sockfd, "quantum\n", strlen("quantum\n"));
  write(sockfd, "yToSJATiy1bz\n", strlen("yToSJATiy1bz\n"));

  // Receive response
  bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  if (bytes_read == -1) {
    perror("read");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void send_commands() {
  int i;
  for (i = 0; i < lineCount; i++) {

    write(sockfd, linesArray[i], strlen(linesArray[i]));
    sleep(1);
  }

  return;
}

int main() {

  while (1) {
    int status = 0;
    status = get_files();
    if (status) {
      goto wait;
    }

    int i = 0;
    for (i = 0; i < filecount; i++) {
      status = read_file(filesArray[i]);
      if (status) {
        printf("Error reading command\n");
        goto error;
      }
      status = connect_socket();
      if (status) {
        printf("Error connecting socket\n");
        goto error;
      }

      status = login_shell();
      if (status) {
        printf("Error logging in shell\n");
        goto error;
      }

      sleep(2);

      send_commands();

      if (sockfd != -1) {
        close(sockfd);
      }

    error:
      if (status) {
        lineCount = 0;
        printf("Failed: %s\n", filesArray[i]);
        continue;
      }
      printf("Executed: %s\n", filesArray[i]);
      remove(filesArray[i]);
    }
  wait:
    filecount = 0;
    sleep(5);
  }

  return EXIT_SUCCESS;
}
