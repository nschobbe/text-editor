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

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

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
  raw.c_cflag |= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)) {
    die("tcgetattr");
  }
}
/*** output ***/

void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b]2J", 4);
}

/*** input ***/

char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread && errno != EAGAIN) {
      die("read");
    }
  } 
  return c;
}

void editorProcessKeypress() {
  char c = editorReadKey();

  switch(c) {
    case CTRL_KEY('q'):
      exit(0);
      break;
  }
}

/*** init ***/

int main() {
  enableRawMode();

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}

