#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

bool running = true;

void clear_screen() {
  printf("\033[2J");
  printf("\033[H");
}

struct termios original;

void restore_terminal(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original); }

void rawmode(void) {

  if (tcgetattr(STDIN_FILENO, &original) == -1) {
    perror("tcgetattr");
    exit(1);
  }

  atexit(restore_terminal);

  struct termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    perror("tcsetattr");
    exit(1);
  }
}

int player_x = 32;
int player_y = 16;

void control() {
  int command = getchar();
  switch (command) {

  case 'w':
    player_y -= 1;
    break;

  case 's':
    player_y += 1;
    break;

  case 'a':
    player_x -= 1;
    break;

  case 'd':
    player_x += 1;
    break;

  case 'q':
    running = false;
    break;
  }
}

int main(void) {

  rawmode();

  // TILES
  char blank = ' ';
  char path = '#';
  char player = '@';
  // TILES

  clear_screen();

  while (running) {
    for (int y = 0; y < 32; y++) {
      for (int x = 0; x < 64; x++) {
        if (x == player_x && y == player_y)
          putchar(player);
        else
          putchar(path);
      }
      printf("\n");
    }

    control();

    sleep(1);

    clear_screen();
  }

  return 0;
}
