// Compare previous and current logs


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE1 "/tmp/logA"
#define FILE2 "/tmp/logB"

char *readfile(char *filename) {
  FILE *file;
  char buffer[256]; // Adjust the buffer size as needed
  int lines_to_skip = 31;
  int line_count = 0;
  char c;
  int i = 0;

  file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening the file");
    return NULL;
  }

  while (line_count < lines_to_skip && fgets(buffer, sizeof(buffer), file)) {
    line_count++;
  }

  long position_before_seek = ftell(file);
  fseek(file, 0, SEEK_END);
  long file_length = ftell(file);
  fseek(file, position_before_seek, SEEK_SET);

  char *string = malloc(sizeof(char) * (file_length + 1));
  while (1) {
    c = fgetc(file);
    if (feof(file)) {
      break;
    }

    string[i] = c;
    i++;
  }

  string[i] = '\0';

  fclose(file);

  return string;
}

void cmp_str(char *str_b, char *str_a) {

  char *found = strstr(str_b, str_a);

  if (found) {
    int length = found - str_b;
    char *string3 = malloc(length + 1);
    strncpy(string3, str_b, length);
    string3[length] = '\0'; // Add null-terminator to the end
    printf("%s", string3);
    free(string3);
  } else {
    printf("No new logs\n");
  }
}

int main() {
  char *str_a = readfile(FILE1);
  char *str_b = readfile(FILE2);

  cmp_str(str_b, str_a);
  return 0;
}
