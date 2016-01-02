import std.conv;
import std.math;
import std.random;

import noteye;

Mt19937 rng;

struct monster {
  int x, y;
  char c;
  int id;
  int foe;
  uint color;
  bool alive;
  }

struct cell {
  int id;
  char terrain;
  }

immutable MAXMON = 105;

monster[MAXMON] mon;
cell[25][80] map;

immutable byte dx[8] = [1, 1, 0, -1, -1, -1, 0, 1];
immutable byte dy[8] = [0, -1, -1, -1, 0, 1, 1, 1];

uint rand() {
  immutable uint n = rng.front;
  rng.popFront;
  return n;
  }

void makemon(int x, int y, char c, uint color, int id) {
  mon[id].x = x;
  mon[id].y = y;
  mon[id].c = c;
  mon[id].id = id;
  mon[id].foe = -1;
  mon[id].color = color;
  mon[id].alive = true;
  map[x][y].id = id;
  }

void createmap() {
  foreach (int y; 2 .. 22)
    foreach (int x; 0 .. 80) {
      map[x][y].id = -1;
      if (y == 2 || y == 21 || x == 0 || x == 79)
        map[x][y].terrain = '#';
      else
        map[x][y].terrain = '.';
      }
  makemon(40, 12, '@', 0xEFFFF00, 0);

  foreach (uint n; 0 .. 104) {
    immutable int x = 25 + (n / 13) + (n / 26) + (n / 52) * 20;
    immutable int y = 6 + (n % 13);
    char c;
    immutable row = n / 26;
    if (row == 0)
      c = 'A' + n % 26;
    else if (row == 1)
      c = 'a' + n % 26;
    else if (row == 2)
      c = 'a' + (n + 13) % 26;
    else if (row == 3)
      c = 'A' + (n + 13) % 26;
    uint color = 0x8808080;
    if (n / 52 == 0)
      color += rand & 0x77F7F00;
    else
      color += rand & 0x7007F7F;
    makemon(x, y, c, color, n + 1);
    }
  }

void drawmap() {
  foreach (int y; 2 .. 22)
    foreach (int x; 0 .. 80) {
      noteye_move(y, x);
      if (map[x][y].id >= 0) {
        immutable id = map[x][y].id;
        setTextAttr32(mon[id].color, 0);
        noteye_addch(mon[id].c);
        continue;
        }
      if (map[x][y].terrain == '#') {
        setTextAttr(8, 0);
        noteye_addch('#');
        continue;
        }
      auto dist = (x - mon[0].x) * (x - mon[0].x);
      dist += (y - mon[0].y) * (y - mon[0].y) * 2;
      if (dist > 200)
        dist = 200;
      setTextAttr32(0xFFFFFFF - 0x10101 * dist, 0);
      noteye_addch('.');
      }
  }

void kill(uint id) {
  map[mon[id].x][mon[id].y].id = -1;
  mon[id].alive = false;
  }

void movemon(int id, uint nx, uint ny) {
  if (map[nx][ny].terrain == '#')
    return;

  auto monid = map[nx][ny].id;
  if (monid != -1) {
    if (isenemy(id, monid))
      kill(monid);
    return;
    }

  map[mon[id].x][mon[id].y].id = -1;
  mon[id].x = nx;
  mon[id].y = ny;
  map[nx][ny].id = id;
  }

void movemon(int id, byte dir) {
  auto nx = mon[id].x + dx[dir];
  auto ny = mon[id].y + dy[dir];

  movemon(id, nx, ny);
  }

bool isenemy(uint me, uint it) {
  return 
       (me <= 52 && it >  52)
    || (me >  52 && it <= 52);
  }

int pickfoe(uint me) {
  uint side;
  if (me <= 52)
    side = 52;
  else
    side = 0;

  uint start = rand % 52;
  foreach (uint n; 0 .. 53) {
    auto id = (n + start) % 53 + side;
    if (mon[id].alive)
      return id;
    }
  return -1;
  }

bool atkaround(uint id) {
  foreach (uint dir; 0 .. 7) {
    auto nx = mon[id].x + dx[dir];
    auto ny = mon[id].y + dy[dir];

    if (map[nx][ny].id == -1)
      continue;

    auto tgt = map[nx][ny].id;
    if (isenemy(id, tgt)) {
      kill(tgt);
      return true;
      }
    }
  return false;
  }

void chasefoe(uint n) {
  auto foe = mon[n].foe;

  if (foe < 0)
    return;

  if (mon[foe].alive == false) {
    mon[n].foe = -1;
    return;
    }

  immutable diffx = mon[foe].x - mon[n].x;
  immutable diffy = mon[foe].y - mon[n].y;
  immutable dx = diffx == 0 ? 0 : diffx / abs(diffx);
  immutable dy = diffy == 0 ? 0 : diffy / abs(diffy);
  movemon(n, mon[n].x + dx, mon[n].y + dy);
  }

void movemons() {
  foreach (uint n; 1 .. MAXMON) {
    if (mon[n].alive == false)
      continue;

    if (atkaround(n))
      continue;

    if (mon[n].foe == -1)
      mon[n].foe = pickfoe(n);

    chasefoe(n);
    }
  }

extern (C) int getmapinfo(lua_State* L) {
  noteye_table_new(L);
  foreach (int y; 2 .. 22) {
    noteye_table_opensubAtInt(L, y);
    foreach (int x; 0 .. 80)
      if (map[x][y].id >= 0) {
        noteye_table_opensubAtInt(L, x);
        noteye_table_setInt(L, "id", map[x][y].id);
        noteye_table_closesub(L);
        }
    noteye_table_closesub(L);
    }
  return 1;
  }

extern (C) void sampleError(int id, const char *b1, const char *b2, int param) {
  }

void status() {
  uint blue, red;
  foreach (uint n; 0 .. MAXMON) {
    if (!mon[n].alive)
      continue;

    if (n > 52)
      ++blue;
    else
      ++red;
    }
  
  string stat = "Red " ~ to!string(red) ~ " : " ~ to!string(blue) ~ " Blue    \0";
  setTextAttr(10, 0);
  noteye_move(24, 35);
  noteye_addstr(cast(char*)stat);
  noteye_move(mon[0].y, mon[0].x);
  if (blue == 0)
    mon[0].alive = false;
  }

void rungame() {
  setTextAttr(10, 0);
  noteye_move(0, 25);
  noteye_addstr("Welcome to Sample Roguelike (D)!");
  noteye_move(23, 15);
  noteye_addstr("Press Ctrl+M to change NotEye modes, 'q' to quit.");

  createmap;
  bool play = true;
  while (mon[0].alive) {
    drawmap;
    status;
    auto c = noteye_getch();
    if (c == 'q') {
      mon[0].alive = false;
      break;
      }

    if (c >= DBASE && c < DBASE+8) {
      immutable byte dir = c & 7;
      movemon(0, dir);
      }

    movemons;
    }
  }

void main(string[] args) {
  rng.seed(unpredictableSeed);

  char[15] arg = "./sample\0";
  char* argp = &arg[0];
  noteye_args(1, &argp);
  noteye_init();
  noteye_globalstr("noteyedir", "../../");
  noteye_globalfun("getmapinfo", &getmapinfo);
  noteye_handleerror(&sampleError);

  noteye_run("../../games/sample.noe", 1);

  rungame();
  noteye_finishinternal(1);
  noteye_uifinish();
  noteye_halt();
  }
