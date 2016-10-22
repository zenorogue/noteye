// Necklace of the Eye v5.0
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

#include <curses.h>

namespace noteye {

static MainScreen *mscr;
static Screen *old;

MainScreen::MainScreen() {
  initscr(); noecho(); keypad(stdscr, true); 
  start_color(); use_default_colors();

  #define COLOR_DEFAULT -1
  
  int cols[9] = {
    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, 
    COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, 
    COLOR_DEFAULT
    };

  for(int u=0; u<81; u++)
    init_pair(u+1, cols[u%9],   cols[u/9]);

  int maxx = 80;
  int maxy = 24;
  getmaxyx(stdscr, maxy, maxx);
  setSize(maxx, maxy);
  
  refresh();
  
  old = new Screen;
  old->setSize(maxx, maxy);
  
  mscr = this;
  }

MainScreen::~MainScreen() {
  endwin(); mscr = NULL;
  }

int tmp;

void col(int x, int y) {
  if(y < 0) y = 8;
  int cb = (x&7) + 9 * y;
  tmp = attrset(COLOR_PAIR(cb+1) | ((x & 8) ?A_BOLD : 0));
  }

//      case KEY_RESIZE:

int colorcmp(int c1, int c2) {
  int sum = 0;
  for(int a=0; a<24; a+=8)
    sum += abs(((c1 >> a) & 0xFF) - ((c2 >> a) & 0xFF));
  return sum;
  }

int findcol(int cfor, int lessthan, int forbidden) {
  if(cfor >> 24) return cfor >> 24;
  if(cfor == 0 && forbidden != 0) return 0;

/*  
  int bp = cfor & 255;
  int gp = (cfor >> 8) & 255;
  int rp = (cfor >> 16) & 255;
  
  int maxv = max(rp, max(gp, bp));
  
  if(maxv < 16) return 8;
  
  int shade = (rp>maxv/2 ?4:0) + (gp>maxv/2?2:0) + (bp>maxv/2?1:0);
  
  if(shade != 7 && (maxv >= 160 || shade == forbidden) && lessthan == 16) return shade + 8;
  else if(shade != 7) return shade;
  else if(maxv > 200 && lessthan == 16) return 15;
  else if(maxv < 130 && lessthan == 16) return 8;
  return 7;
  */

  int bsum = 999, best = 7;
  for(int c=0; c<lessthan; c++) if(c != forbidden) {
    int tsum = colorcmp(cfor, vgacol[c]);
    if(tsum < bsum) bsum = tsum, best = c;
    }
  return best;
  }

#if defined(WINDOWS) && defined(NOCONSOLE)
#define NEEDCONSOLE

#include <Windows.h>

void redirectStd(FILE* f, int which, char *mode) {
  int hnd = _open_osfhandle((long)GetStdHandle(which), _O_TEXT);
  FILE *g = _fdopen(hnd, mode);
  *f = *g;
  setvbuf(f, NULL, _IONBF, 0);
  }
#endif

#ifdef USELUA

int direct_ansi_output;
int ansi_x = -1, ansi_y = -1;
noteyecolor ansi_ba24 = noteyecolor(-2), ansi_cl24 = noteyecolor(-2);

int getansi(int v) { 
  static int ansicodes[8] = {0,4,2,6,1,5,3,7};
  return ansicodes[v&7];
  }
  
int getfaint(int v) { return (v&8) ? 1 : 2; }

static int part5(noteyecolor c, int p) { return (part(c,p)+25) / 51; }

int getxterm256(noteyecolor c) {
  return 16 + part5(c, 0) + part5(c, 1) * 6 + part5(c, 2) * 36;
  }

int lh_refreshconsole(lua_State *L) {
  if(direct_ansi_output) {
    printf("\x1b[?7l\x1b[?20l");
    }
  for(int y=0; y<mscr->sy; y++)
  for(int x=0; x<mscr->sx; x++) {
    int ic = mscr->get(x,y);
    
    if(ic == old->get(x,y)) continue;
    old->get(x,y) = ic;
    
    int ch = getChar(ic);
    noteyecolor ba24 = getBak(ic);
    noteyecolor cl24 = getCol(ic);

    if(ch < 2) ch = 32;
    if(ch == 183) ch = '.';
    if(ch < 32) ch = '$';
    if(ch >= 128) ch = '?';
  
    int ba = int(ba24) == -1 ? -1 : findcol(ba24, 8, -1);
    int cl = int(cl24) == -1 ? 7 : (ba24 != cl24) ? findcol(cl24, 16, ba) : ba;

    if(direct_ansi_output) {
      if(x != ansi_x || y != ansi_y) {
        printf("\x1b[%d;%dH", y+1, x+1);
        ansi_x = x; ansi_y = y;
        }
      
      if(ba24 != ansi_ba24  || cl24 != ansi_cl24) {
        if(int(ba24) == -1)
          printf("\x1b[49m");
        else {
#define SEP if(dosep) printf(";"); else { printf("\x1b["); dosep = true; }
          bool dosep = false;
          if(direct_ansi_output & 1) { SEP printf("4%d", getansi(ba)); }
          if(direct_ansi_output & 2) { SEP printf("48;5;%d",getxterm256(ba24)); }
          if(direct_ansi_output & 4) { SEP printf("48;2;%d;%d;%d", part(ba24,2), part(ba24,1), part(ba24,0)); }
          if(dosep) printf("m");
          }
        if(int(cl24) == -1)
          printf("\x1b[39m");
        else {
          bool dosep = false;
          if(direct_ansi_output & 1) { SEP printf("3%d;%d", getansi(cl), getfaint(cl)); }
          if(direct_ansi_output & 2) { SEP printf("38;5;%d",getxterm256(cl24)); }
          if(direct_ansi_output & 4) { SEP printf("38;2;%d;%d;%d", part(cl24,2), part(cl24,1), part(cl24,0)); }
          if(dosep) printf("m");
          }
#undef SEP
        ansi_ba24 = ba24;
        ansi_cl24 = cl24;
        }
      putchar(ch); ansi_x++;
      fflush(stdout);
      }
    else {
      move(y, x);
      
      // fprintf(stderr, "ba24 = %x cl24 = %x ba = %d cl =%d\n", ba24, cl24, ba, cl);
      
      col(cl, ba);
      // if(ba >= 0) bkgdset(COLOR_PAIR(16 + (ba & 7)));
      addch(ch);
      }
    }
  if(lua_gettop(L) >= 2) {
    if(direct_ansi_output) {
      int y = luaInt(1);
      int x = luaInt(2);
      if(y != ansi_y || x != ansi_x) {
        printf("\x1b[%d;%dH", y+1, x+1);
        ansi_x = x; ansi_y = y;
        }
      }
    else
      move(luaInt(1), luaInt(2));
    }
  if(lua_gettop(L) >= 3) {
    if(direct_ansi_output) {
      if(luaInt(3))
        printf("\x1b[?25h");
      else
        printf("\x1b[?25l");
      curs_set(luaInt(3));
      refresh();
      }
    else
      curs_set(luaInt(3));
    }
  if(!direct_ansi_output) refresh();
  return 0;
  }

int lh_openconsole(lua_State *L) {
  if(mscr) return noteye_retInt(L, mscr->id);

#ifdef NEEDCONSOLE
  AllocConsole();
  redirectStd(stdout, STD_OUTPUT_HANDLE, "w");
  redirectStd(stderr, STD_ERROR_HANDLE, "w");
  redirectStd(stdin, STD_INPUT_HANDLE, "r");
  std::ios::sync_with_stdio(); 
#endif

  return retObjectEv(L, new MainScreen);
  }

void setdirectansi(int val) { direct_ansi_output = val; }

void setconsolewindowtitle(const char *s) {
  printf("\x1b]0;%s\a", s);
  }

#endif

// copied from Hydra Slayer

#ifdef MINGW
#undef getch
#include <conio.h>
#endif

#define C(x) KEY_##x, SDLK_##x, 0,
#define CC(x,y) KEY_##x, SDLK_##y, 0,
#define CF(x) KEY_F0+x, SDLK_F##x, 0, 

#ifdef SDL2
#define CCK(x,y) KEY_##x, SDLK_KP_##y, 0,
#else
#define CCK(x,y) KEY_##x, SDLK_KP##y, 0,
#endif

int curses_to_sdl[] = {
  10, SDLK_RETURN, 0,
  9, SDLK_TAB, 0,
  C(BACKSPACE) C(LEFT) C(RIGHT) C(UP) C(DOWN) C(HOME) C(END)
  CC(PPAGE, PAGEUP) CC(NPAGE, PAGEDOWN) CC(IC, INSERT) CC(DC, DELETE)
  CF(1) CF(2) CF(3) CF(4) CF(5) CF(6) CF(7) CF(8) CF(9) CF(10) CF(11) CF(12)
  CCK(A1, 7) CCK(A3, 9) CCK(C1, 1) CCK(C3, 3) CCK(B2, 5)  
  KEY_SLEFT, SDLK_LEFT, KMOD_RSHIFT,
  KEY_SRIGHT, SDLK_RIGHT, KMOD_RSHIFT
  };

bool MainScreen::checkEvent(lua_State *L) {
  fflush(logfile);
  nodelay(stdscr, true);
  int ch = getch();
  bool alt = false;
  if(ch == 27) {
    ch = getch();
    if(ch > 0) alt = true;
    else ch = 27;
    }
  if(ch > 0) {
    int origch = ch;
    lua_newtable(L);
    noteye_table_setInt(L, "chr", ch);
    int mod = 0;
    bool cooked = false;
    for(int u=0; u<int(sizeof(curses_to_sdl) / sizeof(int)); u+=3)
      if(ch == curses_to_sdl[u]) {
        ch = curses_to_sdl[u+1];
        mod = curses_to_sdl[u+2];
        cooked = true;
        break;
        }
    if(!cooked) {
      const char *low = "1234567890-=[]\\;',./";
      const char *hig = "!@#$%^&*()_+{}|:\"<>?";
      for(int i=0; i<21; i++) if(ch == hig[i]) ch = low[i], mod |= KMOD_LSHIFT;
      if(ch >= 'A' && ch <= 'Z') ch |= 32, mod |= KMOD_LSHIFT;
      else if(ch >= 1 && ch <= 26) ch |= 96, mod |= KMOD_LCTRL;
      else if(ch == KEY_RESIZE) {
        int maxx = 80;
        int maxy = 24;
        getmaxyx(stdscr, maxy, maxx);
        setSize(maxx, maxy);
        return false; // todo: send an event
        }
      }
  
    if(alt) mod |= KMOD_LALT;
    noteye_table_setInt(L, "type", evKeyConsole);
    noteye_table_setInt(L, "sdl", ch);
    noteye_table_setInt(L, "char", origch);
    noteye_table_setInt(L, "mod", mod);
    return true;
    }
  
  return false;
  }

}
