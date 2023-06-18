#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

int pipe_term, pipe_echo;

void end_program() {
  close(pipe_echo);
  close(pipe_term);
  exit(0);
}

int main() {
  signal(SIGINT, end_program);

  // pipe names
  char* pipe_name_term = "NAMED_PIPE_NAME_DZ_OS_TERMINAL";
  char* pipe_name_echo = "NAMED_PIPE_NAME_DZ_OS_ECHO";

  // create echo pipe
  if (unlink(pipe_name_echo) && errno != ENOENT) {
    perror("Pipe occupied:");
    return -1;
  }
  if (mkfifo(pipe_name_echo, 0666)) {
    perror("Cannot make pipe:");
    return -1;
  }

  // connect to pipes
  int pipe_term = open(pipe_name_term, O_RDONLY);
  if (pipe_term < 0) {
    perror("Cannot connect to pipe:");
    return -1;
  }
  int pipe_echo = open(pipe_name_echo, O_WRONLY);
  if (pipe_echo < 0) {
    perror("Cannot connect to pipe:");
    return -1;
  }

  int buffsize = 256;
  char buff[buffsize+1];
  for (;;) {
    int readlen = read(pipe_term, buff, buffsize);
    if (0 == readlen) {
      printf("Terminal process closed\n");
      end_program();
    }
    buff[readlen] = '\0';
    printf("Received: %s", buff);
    if (0 == write(pipe_echo, buff, readlen)) {
      printf("Terminal process closed\n");
      end_program();
    }
  }
}