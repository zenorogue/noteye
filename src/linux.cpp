// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

FILE *ldf;

int lh_ansidebug(lua_State *L) {
  checkArg(L, 1, "debugansi");
  if(ldf) fclose(ldf);
  ldf = fopen(luaStr(1), "wt");
  return 0;
  }

enum ansistate { sNormal, sESC, sCSI, sASK, sCharset };
#define DFT -1

struct LinuxProcess : Process {
  int fd_master;
  int ppid, cpid;
  const char *pts;  // slave pseudoterm
  LinuxProcess(Screen *scr, Font *f, const char *cmdline);
  
  ansistate st;
  int nparam;
  int apar[16];
  int uniwait;
  
  int scurx, scury;

  int brushColor, brushback, brush0;
  int scrollbot, scrolltop;

  bool curvis, curbig, autowrap, autolf, insert, originmode;

  bool bright; int fore, back;
  void setColor();
  
  bool bell; // the bell did ring

  void lf();
  void drawChar(int c);
  int gp(int x, int dft);
  
  void applyM(int i);

  bool checkEvent(lua_State *L);
  void sendRawKeys(const char *buf, int q);
  void sendKey(int scancode, int keycode, int mod, bool down);
  void sendText(const string& s);
  void resetConsole();

  int getCursorSize() {
    if(!curvis) return 0;
    return curbig ? 100 : 1;
    }

  };

Process *startProcess(Screen *scr, Font *f, const char *cmdline) {
  return new LinuxProcess(scr, f, cmdline);
  }

// ----

#include <unistd.h>
#include <fcntl.h>
#ifdef MAC
#include <util.h>
#include <sys/ioctl.h>
#else
#include <pty.h>
#endif
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <utmp.h>

#include <stdlib.h>
extern "C" {
  extern char *ptsname(int fd);
  }

#include <sys/types.h>
#include <sys/wait.h>


#define MAX_ARGS 16

void LinuxProcess::resetConsole() {
  curx = 0; cury = 0; st = sNormal; uniwait = 0;
  back = 0; fore = 7; bright = false; setColor();
  
  for(int x=0; x<s->sx; x++) for(int y=0; y<s->sy; y++)
    s->get(x,y) = brush0;

  scrolltop = 0; scrollbot = s->sy;
  curvis = true; curbig = false;
  }

LinuxProcess::LinuxProcess(Screen *scr, Font *f, const char *cmdline) : Process(scr, f, cmdline) {

  fd_master=posix_openpt(O_RDWR|O_NOCTTY);
  ppid=getpid();  // Process ID of parent
  cpid=-1;    // Get child PID later

  s = scr;

  assert( fd_master >= 0 );  // Did master terminal open?

  assert( unlockpt(fd_master) >= 0 );  // Unlock PTY
  assert( grantpt(fd_master) >= 0 );  // Grant PTY
  
  assert( (pts=ptsname(fd_master)) != NULL ); // Get slave name

  // Create a child process to use the slave pty
  assert( (cpid=fork()) >= 0);

  // set size...
  struct winsize size;
  int ret;
  memset(&size, 0, sizeof(size));
  size.ws_row = scr->sy;
  size.ws_col = scr->sx;
  ret = ioctl(fd_master, TIOCSWINSZ, &size);
  if (ret != 0) {
    logfile && fprintf(logfile, "failed to set window size\n");
    }

  if(cpid == 0) {
    int fd_slave=-1;  // Slave PTY

    assert( close(fd_master) >= 0 ); // Ditch master PTY

    fd_slave=open(pts,O_RDWR);
    
    assert( fd_slave >= 0);

    // This will dup fd over stdin,out,err then close fd
    // This function needs compilation with -lutil
    assert( login_tty(fd_slave) >= 0 );

    // we are simulating the 'linux' terminal
    setenv("TERM", "linux", 1);
    
    // todo: should also disable UTF8

    int s = system(cmdline);
    
    exit(WEXITSTATUS(s));
    }
  
  isActive = true;
  
  resetConsole();
  }

int LinuxProcess::gp(int x, int dft) {
  if(apar[x] == DFT) return dft;
  return apar[x];
  }

void LinuxProcess::lf() {
  cury++;
  if(cury >= scrollbot) {
    for(int y=scrolltop; y<scrollbot-1; y++) for(int x=0; x<s->sx; x++)
      s->get(x, y) = s->get(x, y+1);
    for(int x=0; x<s->sx; x++) s->get(x, scrollbot-1) = brush0;
    cury--;
    }
  }

void LinuxProcess::drawChar(int c) {

  if(c < 0) c += 256;
  
  ldf && fprintf(ldf, "%c", c); ldf && fflush(ldf);
  
  if(st == sNormal) {
    if(c == 0) {
      }
      
    else if(c == 13) { // cr
      curx = 0;
      }
    else if(c == 10) { // lf
      lf();
      }
  
    else if(c == 7) {
      bell = true;
      }

    else if(c == 8) { // del
      if(curx) curx--;
      // s->get(curx, cury) = brush0;
      }
  
    else if(c == 9) { // tab
      do {
        s->get(curx, cury) = brush0;
        curx++;
        if(curx == s->sx) curx = 0, lf();
        }
        while(curx % 8);
      }

    else if(c == 0x1b)
      st = sESC;
    
    else if(0) { // assuming not UTF8 if(c >= 192) {
      uniwait = 0;
      while((c >> (6-uniwait)) & 1) uniwait++;
      s->get(curx, cury) = brush0;
      curx++;
      if(curx == s->sx) curx = 0, lf();
      }
      
    else {
      if(uniwait) { uniwait--; return; }
      if(curx == s->sx) curx = 0, lf();
      s->get(curx, cury) = 
        addMerge(
          brushback,
          addRecolor(f->ti[c], brushColor, 0xffffff),
          false
          );
      curx++;
      }
    }
  
  else if(st == sESC) {
    if(c == '[') {
      st = sCSI;
      nparam = 0; 
      for(int i=0; i<16; i++) apar[i] = DFT;
      }
    else if(c == '(' || c == ')' || c == '+' || c == '*') {
      st = sCharset;
      }
    else if(c == 'M') {
      if(cury == scrolltop) {
        for(int x=0; x<s->sx; x++) for(int y=scrollbot-1; y>=scrolltop; y--)
          if(y > scrolltop) s->get(x,y) = s->get(x, y-1);
          else s->get(x,y) = brush0;
        }
      else cury--;
      st = sNormal;
      }
    else if(c == 'c') {
      resetConsole();
      st = sNormal;
      }
    else if(c == 'D' || c == 'E') {
      if(cury == scrollbot-1) {
        for(int x=0; x<s->sx; x++) for(int y=scrolltop; y<scrollbot; y--)
          if(y < scrollbot-1) s->get(x,y) = s->get(x, y+1);
          else s->get(x,y) = brush0;
        }
      if(c == 'E') curx = 0;
      st = sNormal;
      }
    else if(c == '7') {
      ldf && fprintf(ldf, "{NOTEYE: SAVE}");
      scurx = curx; scury = cury;
      st = sNormal;
      }
    else if(c == '8') {
      ldf && fprintf(ldf, "{NOTEYE: RESTORE}");
      curx = scurx; cury = scury;
      st = sNormal;
      }
    else {
      ldf && fprintf(ldf, "{NOTEYE: ESC-%c}", c);
      st = sNormal;
      }
    }

  else if(st == sCharset) {
    ldf && fprintf(ldf, "{NOTEYE: CHARSET-%c}", c);
    st = sNormal;
    }
  
  else if(st == sCSI) {
    st = sNormal;
    if(c >= '0' && c <= '9') {
      if(apar[nparam] == DFT) apar[nparam] = 0;
      apar[nparam] *= 10;
      apar[nparam] += c - '0';
      st = sCSI;
      }
    else if(c == ';') {
      nparam++; apar[nparam] = DFT;
      st = sCSI;
      }
    else if(c == 'A') {
      cury -= gp(0, 1);
      if(cury < 0) cury = 0;
      }
    else if(c == 'B') {
      cury += gp(0, 1);
      if(cury >= s->sy) cury = s->sy - 1;
      }
    else if(c == 'C') {
      curx += gp(0, 1);
      if(curx >= s->sx) curx = s->sx - 1;
      }
    else if(c == 'D') {
      curx -= gp(0, 1);
      if(curx < 0) curx = 0;
      }
    else if(c == 'E') {
      cury += gp(0, 1);
      if(cury >= s->sy) cury = s->sy - 1;
      curx = 0;
      }
    else if(c == 'F') {
      cury -= gp(0, 1);
      if(cury < 0) cury = 0;
      curx = 0;
      }
    else if(c == 'G') {
      curx = gp(0, 1) - 1;
      if(curx < 0) curx = 0;
      if(curx >= s->sx) curx = 0;
      }
    else if(c == 'd') {
      cury = gp(0, 1) - 1;
      if(cury < 0) cury = 0;
      if(cury >= s->sy) cury = 0;
      }
    else if(c == 'H' || c == 'f') {
      cury = gp(0, 1) - 1;
      curx = gp(1, 1) - 1;
      if(curx < 0) curx = 0;
      if(curx >= s->sx) curx = 0;
      if(cury < 0) cury = 0;
      if(cury >= s->sy) cury = 0;
      }
    else if(c == 'm') {
      for(int i=0; i<=nparam; i++) applyM(apar[i]);
      }
    else if(c == 'K') {
      switch(gp(0, 0)) {
        case 0: for(int x=curx; x<s->sx; x++) s->get(x, cury) = brush0; break;
        case 1: for(int x=0; x<=curx; x++) s->get(x, cury) = brush0; break;
        case 2: for(int x=0; x<s->sx; x++) s->get(x, cury) = brush0; break;
        }
      }
    else if(c == 'M') {
      int q = gp(0, 1);
      for(int x=0; x<s->sx; x++) for(int y=cury; y<scrollbot; y++)
        if(y+q < scrollbot) s->get(x,y) = s->get(x,y+q);
        else s->get(x,y) = brush0;
      }
    else if(c == 'L') {
      int q = gp(0, 1);
      for(int x=0; x<s->sx; x++) for(int y=scrollbot-1; y>=cury; y--)
        if(y-q >= cury) s->get(x,y) = s->get(x,y-q);
        else s->get(x,y) = brush0;
      }
    else if(c == 'P') {
      int q = gp(0, 1);
      for(int x=curx; x<s->sx; x++)
        s->get(x, cury) = x+q >= s->sx ? brush0 : s->get(x+q, cury);
      }
    else if(c == 'X') {
      int q = gp(0, 1);
      for(int i=0; i<q; i++) if(curx + i < s->sx)
        s->get(curx+i, cury) = brush0;
      }
    else if(c == 'J') {
      for(int x=0; x<s->sx; x++) switch(gp(0, 0)) {
        case 0: for(int y=cury; y<s->sy; y++) s->get(x, y) = brush0; break;
        case 1: for(int y=0; y<=cury; y++) s->get(x, y) = brush0; break;
        case 2: for(int y=0; y<s->sy; y++) s->get(x, y) = brush0; break;
        }
      }
    else if(c == '?') {
      st = sASK;
      }
    else if(c == 'r') {
      curx = 0;
      cury = 0;
      scrolltop = gp(0, 1)-1;
      scrollbot = gp(1, s->sy);
      }
    else if(c == 's') {
      ldf && fprintf(ldf, "{NOTEYE: SAVE2}");
      scurx = curx; scury = cury;
      }
    else if(c == 'u') {
      ldf && fprintf(ldf, "{NOTEYE: RESTORE}");
      curx = scurx; cury = scury;
      }
    else {
      if(c < 32)
        ldf && fprintf(ldf, "{NOTEYE CSI: xx [%x]}", c);
      else
        ldf && fprintf(ldf, "{NOTEYE CSI: %c [%x]}", c, c);
      st = sNormal;
      }
    }
  else if(st == sASK) {
    if(c >= '0' && c <= '9') {
      if(apar[nparam] == DFT) apar[nparam] = 0;
      apar[nparam] *= 10;
      apar[nparam] += c - '0';
      }
    else if(c == ';') {
      nparam++; apar[nparam] = DFT;
      }
    else if(c == 'c') {
      if(apar[0] == 8) curbig = true;
      else if(apar[0] == 0) curbig = false;
      else ldf && fprintf(ldf, "{ASKC: %d}\n", apar[0]);
      }
    else if(c == 'l' || c == 'h') {
      for(int i=0; i<=nparam; i++) {
        int n = apar[nparam];
//      if(false) ;
        if(n == 7) autowrap = c == 'h';
        else if(n == 20) autolf = c == 'h';
        else if(n == 4) insert = c == 'h';
        else if(n == 25) curvis = c == 'h';
        else if(n == 6) originmode = c == 'h';
        if(n != 25) 
          ldf && fprintf(ldf, "{NOTEYE %d%c}", n, c);
        }
      st = sNormal;
      // todo?
      }
    else {
      ldf && fprintf(ldf, "{ASK: %c [%x]}\n", c, c);
      st = sNormal;
      }
    }
  }

void LinuxProcess::setColor() {
  int mtrans[8] = {0, 4, 2, 6, 1, 5, 3, 7};
  // brush.back = vgacol[ mtrans[back] ];
  brushColor = vgacol[ mtrans[fore] | (bright ? 8 : 0)];
  brushback = addFill(vgacol[ mtrans[back] ], 0xffffff);
  brush0 = addMerge(brushback, addRecolor(f->ti[32], brushColor, 0xffffff), false);
  }

void LinuxProcess::applyM(int c) {
  if(c == -1) ;
  else if(c == 0) {
    back = 0; fore = 7; bright = false; 
    setColor();
    }
  else if(c == 1) {
    bright = true; setColor();
    }
  else if(c == 7) {
    swap(back, fore); setColor();
    }
  else if(c == 27) {
    swap(back, fore); setColor();
    }
  else if(c == 22) {
    bright = false; setColor();
    }
  else if(c >= 30 && c <= 37) {
    fore = c-30; setColor();
    }
  else if(c == 39) {
    fore = 7; bright = false; setColor();
    }
  else if(c >= 40 && c <= 47) {
    back = c-40; setColor();
    }
  else if(c == 49) {
    back = 0; setColor();
    }
  else if(c >= 10 && c <= 19) {
    }
  else {
    ldf && fprintf(ldf, "{NOTEYE m: %d}", c);
    }
  }

void LinuxProcess::sendRawKeys(const char *buf, int q) {
  if(write(fd_master, buf, q)) 
    ;
  }

void LinuxProcess::sendText(const string& s) {
  sendRawKeys(s.c_str(), s.size());
  }
  

void LinuxProcess::sendKey(int scancode, int keycode, int mod, bool down) {
  if(!down) return;

  int sym = keycode;
  
  // bool shift = mod & (KMOD_LSHIFT|KMOD_RSHIFT);
  // bool ctrl  = mod & (KMOD_LCTRL|KMOD_RCTRL);

  #define Snd(key, x) else if(sym == SDLK_ ## key) sendRawKeys(x, strlen(x));
  #define SndKP(key, x) else if(sym == SDLK_KP_ ## key) sendRawKeys(x, strlen(x));
  
  if(sym == SDLK_LSHIFT) ;
  else if(sym == SDLK_RSHIFT) ;
  else if(sym == SDLK_LCTRL) ;
  else if(sym == SDLK_RCTRL) ;
  else if(sym == SDLK_LALT) ;
  else if(sym == SDLK_RALT) ;
  else if(sym == SDLK_LALT) ;
  else if(sym == SDLK_RALT) ;
  else if(sym == SDLK_CAPSLOCK) ;

  else if(sym == SDLK_RETURN) sendRawKeys("\r", 1);
  Snd(TAB, "\t")

  Snd(F1, "\x1b[[A")
  Snd(F2, "\x1b[[B")
  Snd(F3, "\x1b[[C")
  Snd(F4, "\x1b[[D")
  Snd(F5, "\x1b[[E")
  Snd(F6, "\x1b[17~")
  Snd(F7, "\x1b[18~")
  Snd(F8, "\x1b[19~")
  Snd(F9, "\x1b[20~")
  Snd(F10, "\x1b[21~")
  Snd(F11, "\x1b[23~")
  Snd(F12, "\x1b[24~")

  // linux terminal
  Snd(UP, "\x1b[A")
  Snd(DOWN, "\x1b[B")
  Snd(RIGHT, "\x1b[C")
  Snd(LEFT, "\x1b[D")
  Snd(HOME, "\x1b[1~")
  Snd(END, "\x1b[4~")
  Snd(PAGEUP, "\x1b[5~")
  Snd(PAGEDOWN, "\x1b[6~")

  SndKP(8, "\x1b[A")
  SndKP(2, "\x1b[B")
  SndKP(6, "\x1b[C")
  SndKP(4, "\x1b[D")
  SndKP(7, "\x1b[1~")
  SndKP(1, "\x1b[4~")
  SndKP(9, "\x1b[5~")
  SndKP(3, "\x1b[6~")
  SndKP(5, "\x1b[E")

  #undef Snd
  #undef SndKP

/*
  SndKP(0, "\x1bOp")
  SndKP(1, "\x1bOq")
  SndKP(2, "\x1bOr")
  SndKP(3, "\x1bOs")
  SndKP(4, "\x1bOt")
  SndKP(5, "\x1bOu")
  SndKP(6, "\x1bOv")
  SndKP(7, "\x1bOw")
  SndKP(8, "\x1bOx")
  SndKP(9, "\x1bOy")
*/

  else if(sym >= 'a' && sym <= 'z' && (mod & (KMOD_LCTRL|KMOD_RCTRL))) {
    char retkey = sym - 96;
    sendRawKeys(&retkey, 1);
    }
  
  else if(sym > 0 && sym < 32) {
    char retkey = sym;
    sendRawKeys(&retkey, 1);
    }
  }

#define READBUF 80000

#ifdef USELUA
bool LinuxProcess::checkEvent(lua_State *L) {

  if(!isActive) return false;

  // if(!broken) tty_break(), broken = true;

  fcntl(fd_master, F_SETFL, O_NONBLOCK);
  fcntl(0, F_SETFL, O_NONBLOCK);
  
  char cbuf[READBUF];
  
  if(bell) {
    bell = false;
    lua_newtable(L);
    noteye_table_setInt(L, "type", evBell);
    return true;
    }
    
  bool changed = false;
  
  int rd = read(fd_master, &cbuf, READBUF);

  while(rd > 0) {
    // printf("read %d chars: ", rd);
    /* for(int i=0; i<rd; i++) 
      if(cbuf[i] >= 32) printf("%c", cbuf[i]);
      else printf("[%d]", cbuf[i]); */
    // printf("\n");
    changed = true;

    for(int i=0; i<rd; i++) drawChar(cbuf[i]);
    if(rd < READBUF) rd = 0;
    else rd = read(fd_master, &cbuf, READBUF);
    }  
  
  if(rd < 0 && errno != EAGAIN && errno != EDEADLK) {
    logfile && fprintf(logfile, "errno = %d\n", errno);
    }
  
  int ret = waitpid(-1, &exitcode, WNOHANG);
  if(ret) {
    if(ret != cpid) {
      // to do: fix
      logfile && fprintf(logfile, "Warning: wrong process\n");
      }
    isActive = false;
    lua_newtable(L);
    noteye_table_setInt(L, "type", evProcQuit);
    noteye_table_setInt(L, "obj", id);
    noteye_table_setInt(L, "exitcode", WEXITSTATUS(exitcode));
    return true;
    }

  if(changed) {
    // printf("sending ProcScreen event\n");
    lua_newtable(L);
    noteye_table_setInt(L, "type", evProcScreen);
    noteye_table_setInt(L, "obj", id);
    return true;
    }
  
  return false;
  }
#endif

// IMPORTANT 
/*
  assert( wait(&s) == cpid );
  assert( close(fd_master) >= 0 );
  */

}