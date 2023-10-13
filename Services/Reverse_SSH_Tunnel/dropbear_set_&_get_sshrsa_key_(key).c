/////////////////////////////////////////////////////////////
/// This file use for set & get ssh-rsa key from dropbear /// 
/////////////////////////////////////////////////////////////


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void usage(void) {
  printf("\nUsage:\n"
         "  key [options]\n"
         "\nOptions:\n"
         "  -s      set your public key to authorized_keys\n"
         "  -g      get switch public key\n"
         "  -h      for help\n");
  return;
}

int open_file(char *fname, char *op_type) {
  int file_staus = 0;
  FILE *fp;
  fp = fopen(fname, op_type);
  if (!fp) {
    file_staus = -1;
  }

  return file_staus;
}

int key_get() {
  DIR *dir;
  char command[256];
  char output[256];

  dir = opendir("/mnt/flash/external/ssh");
  if (!dir) {
    mkdir("/mnt/flash/external/ssh", 0755);
  }

  int file_staus = open_file("/mnt/flash/external/ssh/id_dropbear", "r");
  if (!file_staus) {
    goto read_pub_key;
  }

  sprintf(command, "dropbearkey -t rsa -f /mnt/flash/external/ssh/id_dropbear");

  FILE *pipe = popen(command, "r");
  if (pipe == NULL) {
    printf("Error executing command.\n");
    return 1;
  }

  pclose(pipe);

  sprintf(command, "cp /mnt/flash/external/ssh/id_dropbear /root/.ssh/");
  system(command);

read_pub_key:
  sprintf(command, "dropbearkey -y -f /mnt/flash/external/ssh/id_dropbear | "
                   "grep -o 'ssh-rsa.*'");
  pipe = popen(command, "r");
  if (pipe == NULL) {
    printf("Error executing command.\n");
    return 1;
  }

  while (fgets(output, sizeof(output), pipe) != NULL) {
    printf("%s", output);
  }

  return 0;
}

void key_set(char *key) {
  DIR *dir;
  FILE *fp;
  // char *key = malloc(1000);

  dir = opendir("/mnt/flash/external/ssh");
  if (!dir) {
    mkdir("/mnt/flash/external/ssh", 0755);
  }
  fp = fopen("/mnt/flash/external/ssh/authorized_keys", "a");
  if (fp == NULL) {
    return;
  }

  // printf("Please enter your public key: \n");
  // fgets(key, 1000, stdin);
  fputs(key, fp);

  fclose(fp);
  closedir(dir);

  dir = opendir("/root/.ssh");
  if (!dir) {
    mkdir("/root/.ssh", 0755);
  }
  fp = fopen("/root/.ssh/authorized_keys", "w+");
  if (fp == NULL) {
    return;
  }

  fputs(key, fp);
  fclose(fp);
  closedir(dir);

  printf("Okay");

  return;
}

int main(int argc, char **argv) {

  int ch;
  char *ssh_key = malloc(1000);

  if (*argv[1] == 'g') {
    key_get();
  } else {
    strcat(ssh_key, argv[1]);
    strcat(ssh_key, " ");
    strcat(ssh_key, argv[2]);
    strcat(ssh_key, " ");
    strcat(ssh_key, argv[3]);
    key_set(ssh_key);
  }

  return EXIT_SUCCESS;
}
