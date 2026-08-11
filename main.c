#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

//-------------------------------

bool running = true;

char grid[32][64];

int wave_y[128];
int wave_x[128];

int neutral_y[8];
int neutral_x[8];

//-------------------------------

// TILES
char sea = ' ';
char land = '#';
char ship = '@';
char wave = '~';
// TILES

//-------------------------------

void place_waves() {
  for (int i = 0; i < 128; i++) {
    int random_wave_x, random_wave_y;
    do {
      random_wave_y = rand() % 32;
      random_wave_x = rand() % 64;
    } while (grid[random_wave_y][random_wave_x] != sea);

    wave_y[i] = random_wave_y;
    wave_x[i] = random_wave_x;
  }
}

//-------------------------------

void place_neutrals() {
  for (int i = 0; i < 8; i++) {
    int random_neutral_y, random_neutral_x;
    do {
      random_neutral_y = rand() % 32;
      random_neutral_x = rand() % 64;
    } while (grid[random_neutral_y][random_neutral_x] != sea);

    neutral_y[i] = random_neutral_y;
    neutral_x[i] = random_neutral_x;
  }
}

//-------------------------------

void move_waves() {
  for (int i = 0; i < 128; i++) {
    int next_wave_y = wave_y[i];
    int next_wave_x = wave_x[i];

    int random_modulo = rand() % 4;
    switch (random_modulo) {
    case 0:
      next_wave_y += 1;
      break;
    case 1:
      next_wave_y -= 1;
      break;
    case 2:
      next_wave_x += 1;
      break;
    case 3:
      next_wave_x -= 1;
      break;
    }
    if (next_wave_y >= 0 && next_wave_y < 32 && next_wave_x >= 0 &&
        next_wave_x < 64 && grid[next_wave_y][next_wave_x] != land) {
      wave_y[i] = next_wave_y;
      wave_x[i] = next_wave_x;
    } else {
      int random_wave_y, random_wave_x;
      do {
        random_wave_y = rand() % 32;
        random_wave_x = rand() % 64;
      } while (grid[random_wave_y][random_wave_x] != sea);
      wave_y[i] = random_wave_y;
      wave_x[i] = random_wave_x;
    }
  }
}

//-------------------------------

void move_neutrals() {
  for (int k = 0; k < 8; k++) {
    int next_neutral_y = neutral_y[k];
    int next_neutral_x = neutral_x[k];

    int random_modulo = rand() % 4;
    switch (random_modulo) {
    case 0:
      next_neutral_y += 1;
      break;
    case 1:
      next_neutral_y -= 1;
      break;
    case 2:
      next_neutral_x += 1;
      break;
    case 3:
      next_neutral_x -= 1;
      break;
    }
    if (next_neutral_y >= 0 && next_neutral_y < 32 && next_neutral_x >= 0 &&
        next_neutral_x < 64 && grid[next_neutral_y][next_neutral_x] != land) {
      neutral_y[k] = next_neutral_y;
      neutral_x[k] = next_neutral_x;

    } else {
      int random_neutral_y, random_neutral_x;
      do {
        random_neutral_y = rand() % 32;
        random_neutral_x = rand() % 64;
      } while (grid[random_neutral_y][random_neutral_x] != sea);
      neutral_y[k] = random_neutral_y;
      neutral_x[k] = random_neutral_x;
    }
  }
}
//-------------------------------

void clear_screen() {
  printf("\033[2J");
  printf("\033[H");
}

//-------------------------------

struct termios original;

void restore_terminal() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original); }

void rawmode() {

  if (tcgetattr(STDIN_FILENO, &original) == -1) {
    perror("tcgetattr");
    exit(1);
  }

  atexit(restore_terminal);

  struct termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 2;

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
  if (command == EOF) {
    clearerr(stdin);
  }

  switch (command) {

  case 'w':
    if ((player_y > 0) && (grid[player_y - 1][player_x] != land)) {
      player_y -= 1;
    }
    break;

  case 's':
    if ((player_y < 31) && (grid[player_y + 1][player_x] != land)) {
      player_y += 1;
    }
    break;

  case 'a':
    if ((player_x > 0) && (grid[player_y][player_x - 1] != land)) {
      player_x -= 1;
    }
    break;

  case 'd':
    if ((player_x < 63) && (grid[player_y][player_x + 1] != land)) {
      player_x += 1;
    }
    break;

  case 'q':
    running = false;
    break;
  }
}

//-------------------------------

int main() {

  rawmode();

  //-------------------------------

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      grid[y][x] = land;
    }
  }

  //-------------------------------

  int walk_x = 32;
  int walk_y = 16;

  srand(time(nullptr));

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

  place_waves();

  place_neutrals();

  clear_screen();

  while (running) {
    for (int y = 0; y < 32; y++) {
      for (int x = 0; x < 64; x++) {
        bool found_wave = false;
        for (int j = 0; j < 128; j++) {
          if (wave_x[j] == x && wave_y[j] == y) {
            found_wave = true;
          }
        };
        if (x == player_x && y == player_y) {
          printf("\033[33m");
          putchar(ship);
          printf("\033[0m");
        } else if (grid[y][x] == land) {
          printf("\033[32m");
          putchar(land);
          printf("\033[0m");
        } else if (found_wave) {
          printf("\033[34m");
          putchar(wave);
          printf("\033[0m");
        }

        else
          putchar(grid[y][x]);
      }
      printf("\n");
    }

    control();

    move_waves();

    move_neutrals();

    clear_screen();
  }

  return 0;
}

//-------------------------------
