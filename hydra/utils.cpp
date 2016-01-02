// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

#define ANSI
#define CPLUSPLUS

#ifdef ANDROID
#define NOCURSES
#endif

#ifndef ANDROID
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <complex>
using namespace std;
#endif

#ifdef NOTEYE
#define NOCURSES
#define NOTEYE_TRANSLATE
#include "../src/noteye-curses.h"
using namespace noteye;
const char *hydraver = VER;
#define main hydraMain

int getVGAcolorX(int a) { 
  if(a == 18) return 0x2004000;
  if(a == 21) return 0x5FF8000;
  return getVGAcolor(a);
  }

void col(int a) { 
  setTextAttr32(getVGAcolorX(a), getVGAcolor(0));
  }
#endif

#ifndef NOCURSES
#include <curses.h>
#endif

#ifndef ANDROID
void refresh(int context) { refresh(); }
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
//#include <io.h>

string s0;

// string utilities:
string its(int i) {
  char buf[16];
  sprintf(buf, "%d", i);
  return buf;
  }

string itsf(int i, int spc) {
  char buf[10];
  sprintf(buf, "%*d", spc, i);
  return buf;
  }

int bitcount(int x) { return x ? (x&1) + bitcount(x>>1) : 0; }

bool havebit(int set, int bit) { return (set>>bit)&1; }

template<class T> int size(const T& u) { return u.size(); }

// vector utilities:
struct vec2 {
  int32_t x, y;
  vec2(int _x, int _y) { x = _x; y = _y; }
  vec2() {}
  };

vec2 operator+ (vec2 a, vec2 b) { return vec2(a.x+b.x, a.y+b.y); }
vec2 operator- (vec2 a, vec2 b) { return vec2(a.x-b.x, a.y-b.y); }
vec2& operator+= (vec2& a, vec2 b) { a.x += b.x; a.y += b.y; return a; }

int DIRS;
vec2 *dirs;

vec2 operator / (vec2 a, int n) {
  vec2 v = vec2(a.x/n, a.y/n);
  if(DIRS == 6) {
    if((v.x ^ v.y) & 1) {
      if(a.x>0) v.x++; else v.x--;
      }
    }
  return v;
  }
vec2 operator * (vec2 a, int n) { return vec2(a.x*n, a.y*n); }
int len(vec2 v) { 
  int x = abs(v.x); int y = abs(v.y); 
  if(DIRS == 4) return x+y;
  if(DIRS == 8) return x>y ? x : y; 
  if(DIRS == 6) return x>y ? (x+y)/2 : y;
  // impossible
  fprintf(stderr, "wrong directions\n");
  exit(1);
  }

bool operator== (vec2 a, vec2 b) { return a.x == b.x && a.y == b.y; }
bool operator!= (vec2 a, vec2 b) { return !(a==b); }

vec2 dirs4[4] = { vec2(1,0), vec2(0,-1), vec2(-1,0), vec2(0,1) };
vec2 dirs6[6] = { vec2(2,0), vec2(1,-1), vec2(-1,-1), vec2(-2,0), vec2(-1,1), vec2(1,1) };
vec2 dirs8[8] = { vec2(1,0), vec2(1,-1), vec2(0,-1), vec2(-1,-1), vec2(-1,0), vec2(-1,+1), vec2(0,1), vec2(1,1) };
vec2 dirs16[16] = { 
  vec2(2,-1), vec2(1,-2), vec2(-1,-2), vec2(-2,-1), vec2(-2,1), vec2(-1,2), vec2(1,2), vec2(2,1),
  // we just repeat so that the cycle is not disturbed
  vec2(2,-1), vec2(1,-2), vec2(-1,-2), vec2(-2,-1), vec2(-2,1), vec2(-1,2), vec2(1,2), vec2(2,1)
  };

#define MSX 50
#define MSY 22
#define SX  MSX
#define SY  MSY

#ifndef NOCURSES
// Curses utilities:
WINDOW *mainwin;

void initScreen() {
  mainwin = initscr(); noecho(); keypad(stdscr, true); 
  start_color(); use_default_colors();

  #define COLOR_DEFAULT -1
  init_pair(0, COLOR_BLACK,   COLOR_DEFAULT);
  init_pair(1, COLOR_BLUE,    COLOR_DEFAULT);
  init_pair(2, COLOR_GREEN,   COLOR_DEFAULT);
  init_pair(3, COLOR_CYAN,    COLOR_DEFAULT);
  init_pair(4, COLOR_RED,     COLOR_DEFAULT);
  init_pair(5, COLOR_MAGENTA, COLOR_DEFAULT);
  init_pair(6, COLOR_YELLOW,  COLOR_DEFAULT);
  init_pair(7, COLOR_WHITE,   COLOR_DEFAULT);
  init_pair(8, COLOR_BLACK,   COLOR_DEFAULT);
  }

void col(int x) {
  x &= 0xF;
  if(x < 8) attrset(COLOR_PAIR(x));
  else if(x == 8) attrset(COLOR_PAIR(8) | A_BOLD);
  else attrset(COLOR_PAIR(x-8) | A_BOLD);
  }
#endif

void addstri(string s) { addstr(s.c_str()); }

char hydraicon(int heads) {
  if(heads < 34) return ".123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" [heads];
  else if(heads < 100) return 'X';
  else if(heads < 1000) return 'Y';
  else return 'Z';
  }

int powerf(int p) { 
  if(!p) return 0;
  for(int i=0; i<30; i++) if(p <= (1<<i)) return 1+i;
  return 30;
  };

float randf(float min, float max) {
  int r = rand() % 1000;
  r *= 1000;
  r += rand() % 1000;
  return min + (max-min) * (r / 999999.0);
  }

bool squareroot(int x) {
  int i = 0;
  while(i*i < x) i++;
  if(i*i > x) return -1;
  return i;
  }

int primediv(int x) {
  for(int p=2; p*p<=x; p++) if(x%p == 0) return p;
  return -1;
  }

#define SIEVE 20010

vector<short> sieve;

int getPrimeIndex(int x) {
  if(size(sieve) == 0) {
    sieve.resize(SIEVE);
    for(int i=0; i<SIEVE; i++) sieve[i] = true;
    sieve[0] = 0; sieve[1] = 0;
    int had = 0;
    for(int i=2; i<SIEVE; i++) {
      if(sieve[i]) {
        had++;
        for(int j=i; j<SIEVE; j+=i) sieve[j] = false;
        }
      sieve[i] = had;
      }
    }
  if(x >= SIEVE) return -1;
  return sieve[x];
  }

int headlossfun(int x, int level) {
  float hdmax = 10 * level;
  float co = 1/3.;
  if(x < hdmax) return x;
  return int(x / pow(x/hdmax, co) / (1-co) - hdmax * co / (1-co));
  }

string numnames[21] = {
  "zero", "one", "two", "three", "four", "five", "six", "seven", "eight",
  "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
  "sixteen", "seventeen", "eighteen", "nineteen", "twenty"};

string namenum(int x) {
  if(x >= 0 && x < 21) return numnames[x];
  return its(x);
  }

string butlast(string x) {
  return x.substr(0, x.size()-1);
  }

char squareRootSign();

void playSound(const char *fname, int vol = 100, int msToWait = 0);
