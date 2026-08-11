#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

//-------------------------------

bool running = true;

//-------------------------------

void clear_screen() {
  printf("\033[2J");
  printf("\033[H");
}

//-------------------------------

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

//-------------------------------

int player_x = 32;
int player_y = 16;

//-------------------------------

void control() {
  int command = getchar();
  switch (command) {

  case 'w':
    if (player_y > 0) {
      player_y -= 1;
    }
    break;

  case 's':
    if (player_y < 31) {
      player_y += 1;
    }
    break;

  case 'a':
    if (player_x > 0) {
      player_x -= 1;
    }
    break;

  case 'd':
    if (player_x < 63) {
      player_x += 1;
    }
    break;

  case 'q':
    running = false;
    break;
  }
}

//-------------------------------

int main(void) {

  rawmode();

  // TILES
  char sea = ' ';
  char land = '#';
  char ship = '@';
  // TILES

  //-------------------------------

  char grid[32][64];

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      grid[y][x] = land;
    }
  }

  int walk_x = 32;
  int walk_y = 16;

  srand(time(NULL));

  for (int path_steps = 8192; path_steps > 0; path_steps--) {
    grid[walk_y][walk_x] = sea;
    int random_modulo = rand() % 4;
    switch (random_modulo) {
    case 0:
      if (walk_x < 63) {
        walk_x += 1;
      }
      break;
    case 1:
      if (walk_x > 0) {
        walk_x -= 1;
      }
      break;
    case 2:
      if (walk_y < 31) {
        walk_y += 1;
      }
      break;
    case 3:
      if (walk_y > 0) {
        walk_y -= 1;
      }
      break;
    }
  }
  //-------------------------------

  clear_screen();

  while (running) {
    for (int y = 0; y < 32; y++) {
      for (int x = 0; x < 64; x++) {
        if (x == player_x && y == player_y)
          putchar(ship);
        else if (grid[y][x] == land) {
          printf("\033[32m");
          putchar(land);
          printf("\033[0m");
        } else
          putchar(grid[y][x]);
      }
      printf("\n");
    }

    control();

    clear_screen();
  }

  return 0;
}

//-------------------------------
