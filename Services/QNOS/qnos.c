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
#define LOG_FILE "/var/log/qnos/current"

int sockfd;
char buffer[1024];
char *linesArray[500];
int lineCount = 0;
char *filesArray[50];
int filecount = 0;

typedef struct
{
  char name[256];
  time_t timestamp;
} FileEntry;

FileEntry *fileEntries;

void free_memory()
{
  int i;
  for (i = 0; i < lineCount; i++)
  {
    free(linesArray[i]);
  }

  for (i = 0; i < filecount; i++)
  {
    free(filesArray[i]);
  }

  if (fileEntries != NULL)
  {
    free(fileEntries);
  }
}

void log_info(const char *message, const char *filename, const time_t timestamp)
{
  FILE *log_file = fopen(LOG_FILE, "a");
  if (log_file != NULL)
  {
    char formatted_time[30];
    strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d_%H:%M:%S", localtime(&timestamp));
    fprintf(log_file, "%s => %s: Filename: %s : Timestamp: %s\n", formatted_time, message, filename, ctime(&timestamp));
    fclose(log_file);
  }
  else
  {
    perror("fopen");
    printf("Error opening log file for writing\n");
  }
}

int compareFileEntries(const void *a, const void *b)
{
  const FileEntry *fileA = (const FileEntry *)a;
  const FileEntry *fileB = (const FileEntry *)b;

  if (fileA->timestamp < fileB->timestamp)
  {
    return -1;
  }
  else if (fileA->timestamp > fileB->timestamp)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int get_files()
{
  DIR *dir;
  struct dirent *entry;
  struct stat fileStat;
  char path[256];

  if (fileEntries != NULL)
  {
    free(fileEntries);
    fileEntries = NULL;
  }

  dir = opendir(CMD_DIR);
  if (dir == NULL)
  {
    perror("opendir");
    return EXIT_FAILURE;
  }

  while ((entry = readdir(dir)) != NULL)
  {
    snprintf(path, sizeof(path) - 1, "%s/%s", CMD_DIR, entry->d_name);
    path[sizeof(path) - 1] = '\0'; // Ensure null-termination

    if (stat(path, &fileStat) < 0)
    {
      perror("stat");
      return EXIT_FAILURE;
    }

    if (S_ISREG(fileStat.st_mode))
    {
      fileEntries = realloc(fileEntries, (filecount + 1) * sizeof(FileEntry));
      strcpy(fileEntries[filecount].name, entry->d_name);
      fileEntries[filecount].timestamp = fileStat.st_mtime;

      filecount++;
    }
  }

  qsort(fileEntries, filecount, sizeof(FileEntry), compareFileEntries);

  int i = 0;
  char *file = malloc(50);
  for (i = 0; i < filecount; i++)
  {
    sprintf(file, "%s/%s", CMD_DIR, fileEntries[i].name);
    filesArray[i] = strdup(file);
  }

  free(file);
  closedir(dir);

  return EXIT_SUCCESS;
}

int read_file(char *filename)
{
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL)
  {
    perror("fopen");
    return EXIT_FAILURE;
  }

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), fp))
  {
    if (!strcmp(line, "\n"))
    {
      continue;
    }
    char *storedLine = strdup(line);
    linesArray[lineCount] = storedLine;
    lineCount++;
  }

  strcat(linesArray[lineCount - 1], "\n");

  fclose(fp);

  return EXIT_SUCCESS;
}

int connect_socket()
{
  struct sockaddr_un server_address;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    perror("socket");
    return EXIT_FAILURE;
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sun_family = AF_UNIX;
  strncpy(server_address.sun_path, SOCKET_PATH,
          sizeof(server_address.sun_path) - 1);

  if (connect(sockfd, (struct sockaddr *)&server_address,
              sizeof(server_address)) == -1)
  {
    perror("connect");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int login_shell()
{
  ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  if (bytes_read == -1)
  {
    perror("read");
    return EXIT_FAILURE;
  }
  buffer[bytes_read] = '\0';
  printf("%s", buffer);

  write(sockfd, "quantum\n", strlen("quantum\n"));
  write(sockfd, "yToSJATiy1bz\n", strlen("yToSJATiy1bz\n"));

  bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  if (bytes_read == -1)
  {
    perror("read");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void send_commands()
{
  int i;
  for (i = 0; i < lineCount; i++)
  {
    write(sockfd, linesArray[i], strlen(linesArray[i]));
    sleep(1);
  }

  return;
}

int main()
{
  while (1)
  {
    int status = 0;
    status = get_files();
    if (status)
    {
      goto wait;
    }

    int i = 0;
    for (i = 0; i < filecount; i++)
    {
      status = read_file(filesArray[i]);
      if (status)
      {
        printf("Error reading command\n");
        goto error;
      }
      status = connect_socket();
      if (status)
      {
        printf("Error connecting socket\n");
        goto error;
      }

      status = login_shell();
      if (status)
      {
        printf("Error logging in shell\n");
        goto error;
      }

      sleep(2);

      send_commands();

      if (sockfd != -1)
      {
        close(sockfd);
      }

    error:
      if (status)
      {
        lineCount = 0;
        printf("Failed: %s\n", filesArray[i]);
        continue;
      }

      log_info("Executed successfully", fileEntries[i].name, fileEntries[i].timestamp);
      printf("Executed: %s\n", filesArray[i]);
      printf("Filename: %s\n", fileEntries[i].name);
      printf("Timestamp: %s", ctime(&fileEntries[i].timestamp));

      remove(filesArray[i]);
    }
  wait:
    filecount = 0;
    free_memory(); // Free memory before waiting
    sleep(5);
  }
  return EXIT_SUCCESS;
}
