#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

char buffer[1000];
int pipe_term, pipe_echo;

void end_program() {
  close(pipe_echo);
  close(pipe_term);
  exit(0);
}

int main() {
  signal(SIGINT, end_program);
  // pipe names
  char *pipe_name_term = "NAMED_PIPE_NAME_DZ_OS_TERMINAL";
  char *pipe_name_echo = "NAMED_PIPE_NAME_DZ_OS_ECHO";

  // create terminal pipe
  if (unlink(pipe_name_term) && errno != ENOENT) {
    perror("Pipe occupied:");
    return -1;
  }
  if (mkfifo(pipe_name_term, 0666)) {
    perror("Cannot make pipe:");
    return -1;
  }
  // connect to pipes
  pipe_term = open(pipe_name_term, O_WRONLY);
  if (pipe_term < 0) {
    perror("Cannot connect to pipe:");
    return -1;
  }
  pipe_echo = open(pipe_name_echo, O_RDONLY);
  if (pipe_echo < 0) {
    perror("Cannot connect to pipe:");
    return -1;
  }
  
  char *line = NULL;
  size_t len = 0;
  getline(&line, &len, stdin);
  while (strcmp(line, "exit\n")) {
    if (0 == write(pipe_term, line, len)) {
      printf("Echo process closed\n");
      end_program();
    }
    int received = 0;
    char *buff = (char *) malloc(len);
    while (received < len) {
      int readlen = read(pipe_echo, buff, len - received);
      if (0 == readlen) {
        printf("Echo process closed\n");
        end_program();
      }
      received += readlen;
    }
    printf("Received: %s", buff);
    free(buff);
    free(line);
    line = NULL;
    getline(&line, &len, stdin);
  }
}