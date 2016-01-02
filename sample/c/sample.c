// A sample roguelike in pure C, using NotEye as a library.
// See also "games/sample.noe" for the very basic NotEye game configuration.
// It just does nothing -- just uses the defaults and sets the map region.
// See other configs ("games/*.noe") to know how to do things.

#include <stdlib.h>

#include "../../src/noteye.h"
#include "../../src/noteye-curses.h"

int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

#define TRUE 1 // you should just use 'true' in C++

struct monster {
  int x, y;
  char c;
  int color;
  int id;
  };

typedef struct monster monster;

#define MAXMON 100

monster *mon[MAXMON];

char gamemap[25][80];

int getmapinfo(lua_State* L) {
  noteye_table_new(L);
  int i, y;
  for(y=2; y<22; y++) {
    noteye_table_opensubAtInt(L, y);
    for(i=0; i<100; i++) if(mon[i] && mon[i]->y == y) {
      noteye_table_opensubAtInt(L, mon[i]->x);
      noteye_table_setInt(L, "id", i);
      noteye_table_closesub(L);
      }
    noteye_table_closesub(L);
    }
  return 1;
  }

monster *getmonat(int x, int y) {
  int i;
  for(i=0; i<MAXMON; i++) if(mon[i] && mon[i]->x == x && mon[i]->y == y) {
    mon[i]->id = i;
    return mon[i];
    }
  return NULL;
  }

monster *newmonster(int x, int y, char c, int color) {
  monster *m = malloc(sizeof(monster));
  m->x = x; m->y = y;
  m->c = c; m->color = color;
  return m;
  }

int sx, sy;

void createmap() {
  int i, x, y;
  sx = noteye_getinternalx();
  sy = noteye_getinternaly();
  
  if(sx > 80) sx = 80;
  if(sy > 25) sy = 25;
  
  for(y=2; y<22; y++) for(x=0; x<sx; x++) gamemap[y][x] = '#';
  for(y=3; y<21; y++) for(x=1; x<sx-1; x++) gamemap[y][x] = '.';

  mon[0] = newmonster(40, 12, '@', 0xEFFFF00);
  for(i=1; i<100; i++) {
    int x, y;
    do {
      x = 1 + rand() % 78;
      y = 3 + rand() % 18;
      }
    while(getmonat(x,y));
    mon[i] = newmonster(x, y, 'A' + rand() % 26, 0x8808080 + (rand() & 0x77F7F7F));
    }
  }

void drawmap() {
  int x, y, i;

  for(y=2; y<22; y++)
  for(x=0; x<sx; x++) {
    noteye_move(y, x);
    if(gamemap[y][x] == '#')
      setTextAttr(8, 0);
    else {
      int dist = (x-mon[0]->x)*(x-mon[0]->x) + (y-mon[0]->y)*(y-mon[0]->y);
      if(dist > 200) dist = 200;
      // we use true color here
      setTextAttr32(0xFFFFFFF - 0x10101*dist, 0);
      }  
    noteye_addch(gamemap[y][x]);
    }

  for(i=0; i<100; i++) if(mon[i]) {
    noteye_move(mon[i]->y, mon[i]->x); setTextAttr32(mon[i]->color, 0);
    noteye_addch(mon[i]->c);
    }    
  
  noteye_move(mon[0]->y, mon[0]->x);
  }

void movemonsters() {
  int i;
  for(i=1; i<100; i++) if(mon[i]) {
    int ax = mon[i]->x - mon[0]->x;
    int ay = mon[i]->y - mon[0]->y;
    if(ax<0) ax=-ax;
    if(ay<0) ay=-ay;
    if(ax<=1 && ay<=1) {
      mon[0]->color = 0x4600000;
      continue;
      }
    int d = rand() % 8;
    int nx = mon[i]->x + dx[d];
    int ny = mon[i]->y + dy[d];
    if(getmonat(nx, ny)) continue;
    if(gamemap[ny][nx] == '#') continue;
    mon[i]->x += dx[d];
    mon[i]->y += dy[d];
    }
  }

// this function runs the "game"
void rungame() {
  int i;

  // Actually, noteye_curses.h defines abbreviations, so you can
  // simply write "move" as in Curses instead of noteye_move
  // however, we use full names in this sample.

  // 10 = light green in DOS, 0 = black background
  setTextAttr(10, 0);
  noteye_move(0, 25);
  noteye_addstr("Welcome to Sample Roguelike (C)!");
  noteye_move(23, 15);
  noteye_addstr("Press Ctrl+M to change NotEye modes, 'q' to win.");
  noteye_move(24, 15);
  noteye_addstr("F8 causes the NotEye script to crash.");
  
  createmap();
  
  while(TRUE) {
  
    drawmap();
    
    int ch = noteye_getch();
    
    // printf("ch = %d mod = %x\n", ch, noteye_getlastkeyevent()->key.keysym.mod);
    
    if(ch >= DBASE && ch < DBASE+8) {
      int d = ch & 7;
      monster *m = getmonat(mon[0]->x+dx[d], mon[0]->y+dy[d]);
      if(m) {
        mon[m->id] = NULL;
        free(m);
        movemonsters();
        }
      else {
        mon[0]->x += dx[d];
        mon[0]->y += dy[d];
        movemonsters();
        }
      }
    
    if(ch == DBASE+8 || ch == '.' || ch == ' ')
      movemonsters();
    
    if(ch == 'q') {
      printf("You have won!\n");
      break;
      }

    if(ch == NOTEYEERR) {

      // in some cases, noteye_getch() will return NOTEYEERR
      // after the script causes an error.
      // Restart NotEye is the only thing we can do then
      
      printf("\n\nNotEye error detected, restarting NotEye\n");

      noteye_halt();

      noteye_init();
      noteye_globalstr("noteyedir", "../../");
      noteye_globalfun("getmapinfo", getmapinfo);
      noteye_run("../../games/sample.noe", TRUE);

      printf("restart OK\n");
      // we do not refresh the screen, so titles do not appear
      }
    }
  }

void sampleError(int id, const char *b1, const char *b2, int param) {
  fprintf(stderr, "handling NotEye error #%d: %s", id, b1);
  if(b2) fprintf(stderr, " [%s]", b2);
  if(param != NOPARAM) fprintf(stderr, " [%d]", param);
  fprintf(stderr, "\n");
  }

int main(int argc, char **argv) {
  srand(time(NULL));
  
  // the arguments are simply copied to the NotEye scripts
  noteye_args(argc, argv);  
  
  // initialize NotEye
  noteye_init();
  
  // initialize NotEye directories
  noteye_globalstr("noteyedir", "../../");
  noteye_globalfun("getmapinfo", getmapinfo);
  
  // handle errors
  noteye_handleerror(sampleError);

  // We could add additional Lua functions here. For example, Hydra Slayer
  // adds a function to give a complete information about the given cell
  // (more detailed than the ASCII character), and a function to give help
  // about the given cell (clicked with mouse). These functions are called
  // from the Lua script directly.

  noteye_run("../../games/sample.noe", TRUE);

  // we follow the "threaded" approach. This means that UI runs in a
  // separate thread (Lua coroutine, actually). Hydra Slayer follows
  // another route -- the game is called from inside games/hydra.noe,
  // and the script works during getch().  
  rungame();  
  // tell the script that the game is finished
  noteye_finishinternal(1);
  // close the UI thread
  noteye_uifinish();
  // finish NotEye (this also exits the process)
  noteye_halt();
  }
