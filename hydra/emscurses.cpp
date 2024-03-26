// Emscripten implementation of Curses.

#include <emscripten.h>

#include <string>
#include <functional>

using std::string;

int currentscr;

int halfdelaymode = -1;
int cursorsize = 1;

void halfdelay(int i) { halfdelaymode = 100 * i; }
void halfdelayms(int i) { halfdelaymode = i; }
void cbreak() { halfdelaymode = -1; }
void curs_set(int i) { if(i==2) i=100; cursorsize = i; }
void curs_setx(int i) { cursorsize = i; }

struct Ptype {
  static const int sx = 80, sy = 25;
  int curx, cury, curcol;
  bool changed;
  char text[sy][sx];
  int colors[sy][sx];
  string webkey[sy][sx];
  } Px;

#define P (&Px)  

int hscols[16] = {
  0x0000000, 0x10F52BA, 0x23C965A, 0x3188484, 
  0x4841B2D, 0x5664488, 0x6964B00, 0x78C8C8C, 
  0x851484F, 0x971A6D2, 0xA32CD32, 0xB7FFFD4, 
  0xCff5555, 0xDE164D1, 0xEFFD700, 0xFC0C0C0
  };

void col(int c) { 
  unsigned col;

  if(c == 18) col = 0x2004000;
  else if(c == 20) col = 0x4000000 + 0x764e7c*2;
  else if(c == 21) col = 0x5FF8000;
  else col = hscols[c&15];
  
  P->curcol = col;
  }

void refresh() { }

void addch(char ch) {
  if(!P) return;
  P->changed = true;
  if(ch == '\n') {
    P->curx = 0;
    if(P->cury < P->sy-1) P->cury++;
    return;
    }
  if(P->curx < P->sx) {
    P->colors[P->cury][P->curx] = P->curcol;
    P->text[P->cury][P->curx] = ch;
    P->curx++;
    if (P->curx >= P->sx) {
      P->curx = 0;
      if(P->cury < P->sy-1) P->cury++;
      }
    }
  }

void addstr(string s) { for(char c: s) addch(c); }

void move(int y, int x) { 
  P->curx = x;
  P->cury = y;
  }

void endwin() { }

void erase() { 
  P->changed = true;
  for(int y=0; y<P->sy; y++) for(int x=0; x<P->sx; x++) 
    P->text[y][x] = ' ',
    P->webkey[y][x] = "";
  P->curx=0; P->cury=0;
  }

void line_webkey(string s) {
  for(int x=P->curx; x<P->sx; x++) 
    P->webkey[P->cury][x] = s;
  }
  
void clrtoeol() { 
  P->changed = true;
  for(int x=P->curx; x<P->sx; x++) 
    P->text[P->cury][x] = ' ';
  }


void initScreen() { }

#define KEY_F0 300

string get_value(string name) {
  char *str = (char*)EM_ASM_INT({
    var name = UTF8ToString($0, $1);
    var value = document.getElementById(name).value;
    var lengthBytes = lengthBytesUTF8(value)+1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(value, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
    }, name.c_str(), int(name.size())
    );
  string res = str;
  free(str);
  return res;
  }

void set_value(string name, string s) {
  EM_ASM_({
    var name = UTF8ToString($0, $1);
    var value = UTF8ToString($2, $3);
    document.getElementById(name).innerHTML = value;
    }, name.c_str(), int(name.size()),
    s.c_str(), int(s.size())
    );
  }

key_continuation keyhandler = [] (int c) { set_value("all", "thanks for playing!"); };

void INIC(int x);

#define KH(c,x) INIC(x); keyhandler = [=] (int c)
#define NOEMS(x)
#define ONEMS(x) x

void ems_main(void_continuation vcon);

bool gameoff = false;

void update_screen() {
  if(gameoff) return;
  string tt = "<pre style=\"font-size:20px; background-color:black;\"><font color=#FFFFFF>\n";
  
  int lcolor = 15;
  char buf[100];
  string lwk = "";

  for(int y=0; y<P->sy; y++) {
    for(int x=0; x<P->sx; x++) {
      int ncolor = P->colors[y][x] & 0xFFFFFF; // getCol(P->get(x,y)) & 0xFFFFFF;
      char c = P->text[y][x];
      if(c == 0) c = 32;
      string webkey = P->webkey[y][x];
      if(webkey != lwk) {
        tt += "</font>";
        if(lwk != "") tt += "</span>";
        lwk = webkey;
        if(lwk != "") {
          tt += "<span onclick=\"webkey('" + lwk + "');\">";
          }
        lcolor = ncolor;
        sprintf(buf, "<font color=#%06x>", ncolor);
        tt += buf;
        }
      if(ncolor != lcolor && c != 32) {
        sprintf(buf, "</font><font color=#%06x>", ncolor);
        tt += buf;
        lcolor = ncolor;
        }
      tt += c;
      }
    tt += "\n";
    }
  if(lwk != "") tt += "</span>";
  tt += "</font></pre></html>\n";

  set_value("all", tt);
  }

void websync() {
  EM_ASM({
    FS.syncfs(false, function(err) {
      document.getElementById("log").innerHTML = (err ? "save error" : "data saved successfully");
      })
    });
  }

void quit_webgame() {
  gameoff = true;
  set_value("all", "thanks for playing!"); 
  set_value("keytable", "");
  keyhandler = [] (int c) {};
  };

extern "C" {
  void doit(int i) {
    ems_main(quit_webgame);
    update_screen();
    }

  bool webkey(const char *input) {
    string inp = input;
    if(inp == "Enter")
      keyhandler('\n');
    else if(inp == "Backspace")
      keyhandler(8);
    else if(inp == "Tab")
      keyhandler(9);
    else if(inp == "F1")
      keyhandler(KEY_F0+1);
    else if(inp == "ArrowRight")
      keyhandler(D_RIGHT);
    else if(inp == "ArrowLeft")
      keyhandler(D_LEFT);
    else if(inp == "ArrowUp")
      keyhandler(D_UP);
    else if(inp == "ArrowDown")
      keyhandler(D_DOWN);
    else if(inp == "PageDown")
      keyhandler(D_PGDN);
    else if(inp == "PageUp")
      keyhandler(D_PGUP);
    else if(inp == "Home")
      keyhandler(D_HOME);
    else if(inp == "PageUp")
      keyhandler(D_END);
    else if(inp == "Escape")
      keyhandler(ESC);
    else if(inp == "F2")
      keyhandler(KEY_F0+2);
    else if(inp == "F10")
      keyhandler(KEY_F0+10);
    else if(inp.size() == 1)
      keyhandler(inp[0]);
    else {
      set_value("log", "unknown key: " + inp);
      return false;
      }
    update_screen();
    return true;
    }
  
  void eachtick() {
    // if(halfdelaymode > 0) 
    if(halfdelaymode > 0) keyhandler(0), update_screen();
    }

  }

#undef P
