#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

static struct __attribute__((packed)) saved_state {
  int ai_wins, player_wins;
  int ai_hits, player_hits;
  int ai_misses, player_misses;
  int games;
  uint32_t rand_state;

  char playername[20];
  char validationkey[100];
  char player_grid[10][10];
  char computer_grid[10][10];
} state;

int32_t my_rand() {
  state.rand_state = ((uint64_t)state.rand_state * 48271) % 2147483647;
  return (int32_t)state.rand_state;
}

struct shipdat {
  char name[15];
  char size;
} shipdat[] = {
  {"Carrier\x00",    5},
  {"Battleship\x00", 4},
  {"Submarine\x00",  3},
  {"Destroyer\x00",  3},
  {"Patrol boat\x00", 2}
};

void printf_with_shipchar(char *format, char ship) {
  char buf[2] = {ship, 0};
  const char *name = buf;
  int i;
  for(i = 0; i < sizeof(shipdat)/sizeof(struct shipdat); i++) {
    if(shipdat[i].name[0] == ship) {
      name = shipdat[i].name;
      break;
    }
  }
  printf(format, name);
}

void computer_attack() {
  int i;
  for(i = 0; i <= 1000; i++) {
    int x = my_rand()%10;
    int y = my_rand()%10;
    char c = state.player_grid[x][y];
    if(c == '0' || c == 'X') continue;
    if(c == ' ') {
      state.player_grid[x][y] = '0';
      puts("The computer player misses.");
      state.ai_misses++;
    } else {
      state.player_grid[x][y] = 'X';
      printf_with_shipchar("The computer player hits your %s.\n", c);
      state.ai_hits++;
    }
    return;
  }
}

void get_line(char *buf, int size) {
  int i;
  for(i = 0; i < size; i++) {
    char c = getc(stdin);
    if(c == EOF) exit(0);
    if(c == '\n') {
      if(i >= 1 && buf[i-1] == '\r') {
        buf[i-1] = 0;
      } else {
        buf[i] = 0;
      }
      return;
    }
    buf[i] = c;
  }
  
  if(buf[size-1] == '\r') {
    buf[size-1] = getc(stdin);
    if(buf[size-1] == '\n') {
      buf[size-1] = 0;
    } else {
      ungetc(buf[size-1], stdin);
      ungetc('\r', stdin);
    }
  } else {
    ungetc(buf[size-1], stdin);
    buf[size-1] = 0;
  }
}

void print_board(int show_all) {
  int row;
  int col;
  printf("\n %-20s     Computer\n", state.playername);
  printf("  1-2-3-4-5-6-7-8-9-10     1-2-3-4-5-6-7-8-9-10\n");
  for(row = 0; row < 10; row++) {
    printf("%c|", "ABCDEFGHIJ"[row]);
    for(col = 0; col < 10; col++) {
      char c = state.player_grid[row][col];
      if(c == 0) c = ' ';
      printf("%c|", c);
    }
    printf("   %c|", "ABCDEFGHIJ"[row]);
    for(col = 0; col < 10; col++) {
      if(show_all) {
        char c = state.computer_grid[row][col];
        if(c == 0) c = ' ';
        printf("%c|", c);
      } else {
        char c = state.computer_grid[row][col];
        if(c != 'X' && c != '0') c = ' ';
        printf("%c|", c);
      }
    }
    printf("\n");
  }
  printf("  -------------------      -------------------\n\n");
}

void scramble(void *buf, int len) {
  int i;
  srand(999);
  for(i = 0; i < len; i++)
    ((unsigned char *)buf)[i] ^= rand()&0xff;
}

void computer_setup() {
  int i;
  int e;
  memset(state.computer_grid, ' ', sizeof(state.computer_grid));
  for(i = 0; i < sizeof(shipdat)/sizeof(struct shipdat); i++) {
again:;
    int lo = my_rand() % (10-shipdat[i].size);
    int hi = lo + shipdat[i].size;
    int r = my_rand() % 10;
    if(my_rand() & 1) {
      for(e = lo; e < hi; e++) {
        char q = state.computer_grid[r][e];
        if(q && q != ' ') goto again;
      }
      for(e = lo; e < hi; e++) {
        state.computer_grid[r][e] = shipdat[i].name[0];
      }
    } else {
      for(e = lo; e < hi; e++) {
        char q = state.player_grid[e][r];
        if(q && q != ' ') goto again;
      }
      for(e = lo; e < hi; e++) {
        state.computer_grid[e][r] = shipdat[i].name[0];
      }
    }
  }
}

int player_setup() {
  char buf[100];
  char *dash;
  int i;
  int e;
  
  memset(state.player_grid, ' ', sizeof(state.player_grid));
  puts("Board setup.");
  
  for(i = 0; i < sizeof(shipdat)/sizeof(struct shipdat); i++) {
again:;
      printf("Place your ");
      printf(shipdat[i].name);
      printf(" now.\n> ");
      get_line(buf, sizeof(buf));
      if(memcmp(buf, "quit", 5) == 0) return 0;
      
      char *dash = strchr(buf, '-');
      if(!dash) {
        printf("Error: Bad format, use something like B10-%c10.\n", "BCDEFGH"[shipdat[i].size-1]);
        goto again;
      }
      
      if(dash[1] < 'A' || 'J' < dash[1]) {
        memmove(&dash[2], &dash[1], strlen(&dash[1])+1);
        dash[1] = buf[0];
      }
      
      if(dash[2] < '0' || '9' < dash[2]) {
        char *src = &buf[1];
        char *dst = &dash[2];
        int e = dash - &buf[1];
        while(e-- > 0) *(dst++) = *(src++);
        *dst=0;
      }
      
      *dash = 0;
      int a = buf[0] - 'A';
      int b = atoi(&buf[1]) - 1;
      int c = dash[1] - 'A';
      int d = atoi(&dash[2]) - 1;
      
      int lo, hi;
      if(a == c) {
        if(b < d) {
          lo = b, hi = d+1;
        } else {
          lo = d, hi = b+1;
        }
        if(shipdat[i].size != hi - lo) {
          printf("Error: The %s is %d long, not %d!\n", shipdat[i].name, shipdat[i].size, hi - lo);
          goto again;
        }
        
        for(e = lo; e < hi; e++) {
          char q = state.player_grid[a][e];
          if(q && q != ' ') {
            printf_with_shipchar("Error: You're overlapping a %s.\n", q);
            goto again;
          }
        }
        if(lo < 0 || hi > 10 || a < 0 || a > 9) {
          puts("Error: Placement is out of bounds!");
          goto again;
        }
        for(e = lo; e < hi; e++) {
          state.player_grid[a][e] = shipdat[i].name[0];
        }
      }
      else if(b == d) {
        if(a < c) {
          lo = a, hi = c+1;
        } else {
          lo = c, hi = a+1;
        }
        if(shipdat[i].size != hi - lo) {
          printf("Error: The %s is %d long, not %d!\n", shipdat[i].name, shipdat[i].size, hi - lo);
          goto again;
        }
        
        for(e = lo; e < hi; e++) {
          char q = state.player_grid[e][b];
          if(q && q != ' ') {
            printf_with_shipchar("Error: You're overlapping a %s.\n", q);
            goto again;
          }
        }
        if(lo < 0 || hi > 10 || b < 0 || b > 9) {
          puts("Error: Placement is out of bounds!");
          goto again;
        }
        for(e = lo; e < hi; e++) {
          state.player_grid[e][b] = shipdat[i].name[0];
        }
      }
      else {
        puts("Error: Ships must be vertical or horizontal!");
        goto again;
      }
  }
  return 1;
}

int player_attack() {
again:;
  puts("Choose target");
  printf("> ");
  char buf[100];
  get_line(buf, sizeof(buf));
  if(memcmp(buf, "quit", 5) == 0) return 0;
  int a = buf[0] - 'A';
  int b = atoi(&buf[1]) - 1;
  if(a < 0 || a > 9 || b < 0 || b > 9) {
    puts("Error: Invalid target!");
    goto again;
  }
  char c = state.computer_grid[a][b];
  if(c == 'X' || c == '0') {
    puts("You fruitlessly strike an already attempted target.");
    state.player_misses++;
    return 1;
  }
  if(c == 0 || c == ' ') {
    state.computer_grid[a][b] = '0';
    puts("You miss.");
    state.player_misses++;
  } else {
    state.computer_grid[a][b] = 'X';
    printf_with_shipchar("You hit a %s!\n", c);
    state.player_hits++;
  }
  return 1;
}

int board_defeated(char board[10][10]) {
  int x;
  int y;
  for(x = 0; x < 10; x++) {
    for(y = 0; y < 10; y++) {
      char c = board[x][y];
      if(c != 0 && c != ' ' && c != 'X' && c != '0') {
        return 0;
      }
    }
  }
  return 1;
}

void play_game() {
  char buf[10];
  if(!player_setup()) {
    puts("Nevermind then.");
    return;
  }
  computer_setup();
  
  puts("Let the games begin!");
  
  if(my_rand()&1) {
    puts("You're up first.");
    goto human_first;
  } else {
    puts("Computer moves first.");
  }
  
  state.games++;
  while(1) {
    computer_attack();
    if(board_defeated(state.player_grid)) {
      state.ai_wins++;
      puts("Computer wins, u ded.");
      break;
    }
human_first:
    print_board(0);
    if(!player_attack()) {
      puts("Quitter!");
      state.ai_wins++;
      return;
    }
    if(board_defeated(state.computer_grid)) {
      state.player_wins++;
      puts("VICTORY");
      break;
    }
  }
  
  print_board(1);
  printf("...");
  get_line(buf, 100);
}

int main(int argc, char *argv[]) {
  setbuf(stdin, 0);
  setbuf(stdout, 0);
  puts("Welcome to the Battleship game!");
  puts("Who are you, anyways?");
  printf("> ");
  get_line(state.playername, sizeof(state.playername));
  printf("Hi %s. Type \"play\" to play.\n", state.playername);
  
  // Don't change. Determinism is part of the SLA checking and
  // it doesn't really lead to any (intended) vulnerabilities.
  state.rand_state = 1;
  
  while(1) {
    char buf[100];
    printf("> ");
    get_line(buf, sizeof(buf));
    if(memcmp(buf, "play", 5) == 0) {
      play_game();
      puts("Type \"play\" to play again, or \"stats\" to see how you're doing.");
    }
    else if(memcmp(buf, "stats", 6) == 0) {
      printf("You've won %d/%d games, hitting %d times and missing %d times.\n", state.player_wins, state.games, state.player_hits, state.player_misses);
      printf("Computer has won %d/%d games, hitting %d times and missing %d times.\n", state.ai_wins, state.games, state.ai_hits, state.ai_misses);
    }
    else if(memcmp(buf, "quit", 5) == 0) {
      if(state.player_wins < state.ai_wins) {
        puts("See ya, loser!");
      } else {
        puts("See ya!");
      }
      return 0;
    }
    else if(memcmp(buf, "rename ", 7) == 0) {
      strncpy(state.playername, buf+7, sizeof(state.playername));
    }
    else if(memcmp(buf, "load", 5) == 0) {
      FILE *f = fopen("./key", "r");
      if(!f) {
        perror("fopen");
        exit(-1);
      }
      int x = fread(buf, 1, sizeof(buf)-1, f);
      buf[x] = 0;
      fclose(f);
      
      fread(&state, sizeof(struct saved_state), 1, stdin);
      
      if(strcmp(state.validationkey, buf) == 0) {
        puts("Validation key used, accepting loaded state!");
        scramble(&state, sizeof(struct saved_state));
        strcpy(state.validationkey, buf);
        scramble(&state, sizeof(struct saved_state));
      }
      
      scramble(&state, sizeof(struct saved_state));
      if(strcmp(state.validationkey, buf) == 0) {
        printf("Welcome back %s.\n", state.playername);
      } else {
        printf("I didn't make that dump recently! Go away!\n");
        return 0;
      }
    }
    else if(memcmp(buf, "dump", 5) == 0) {
      FILE *f = fopen("./key", "r");
      if(!f) {
        perror("fopen");
        exit(-1);
      }
      int x = fread(state.validationkey, 1, sizeof(state.validationkey)-1, f);
      state.validationkey[x] = 0;
      fclose(f);
      
      scramble(&state, sizeof(struct saved_state));
      fwrite(&state, sizeof(struct saved_state), 1, stdout);
      scramble(&state, sizeof(struct saved_state));
    }
    else {
      puts("What?");
    }
  }
}
