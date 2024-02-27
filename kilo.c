// Kilo text editor - tutorial from https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
// Author: Neal Schobbe
// Feb 2024

/*** includes ***/

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)) {
    die("tcsetattr");
  }
}

void enableRawMode() {
  if(tcgetattr(STDIN_FILENO, &orig_termios)) {
    die("tcgetattr");
  }

  atexit(disableRawMode);

  struct termios raw = orig_termios;
  //setting flags to disable various ctrl-<key> combos
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_clfag |= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.cc[VMIN] = 0;
  raw.cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)) {
    die("tcgetattr");
  }
}

/*** init ***/

int main() {
  enableRawMode();

  char c = '\0';
  while (1) {
    c = '\0'
    if (read(STDIN_FILENO, &c, 1) && errno != EAGAIN) {
      die("read");
    }
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') {
      break;
    }
  }

  return 0;
}

