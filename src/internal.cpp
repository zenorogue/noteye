// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

// 'Internal process', used for integration.

#define CURSES_CONSTONLY

#include "noteye-curses.h"

namespace noteye {


lua_State *internalstate;
lua_State *uithread;
lua_State *uithread_out;

bool uithread_err; // there was an error with uithread

bool uithread_running; // the UI thread is currently running

// ----

#include <unistd.h>
#include <fcntl.h>
// #include <pty.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
// #include <utmp.h>

#include <stdlib.h>

#include <sys/types.h>
// #include <sys/wait.h>


#define MAX_ARGS 16

InternalProcess *P;

#define KEY_F0 256

extern "C" {

void initScreen() {
  }

void noteye_refresh() {
  if(uithread && !uithread_running) {
    if(uithread_err) return;
    uithread_running = true;
    int status = lua_resume(uithread,0);
    luamapstate = uithread_out;
    uithread_running = false;
    if(status != LUA_YIELD) {
      noteyeError(8, "error: did not yield", lua_tostring(uithread, -1), status);
      uithread_err = true;
      }
    }
  
  else if(uithread) {
    lua_getglobal(uithread, CALLUI);
    // lua_pushvalue(internalstate, 2);
    int status = lua_pcall(uithread, 0, 1, 0);
    if (status != 0) {
      noteyeError(9, "error running mainloopcyc", lua_tostring(uithread, -1), status);
      }
    lua_pop(uithread, 1);
    }

  else {      
    lua_getglobal(internalstate, CALLUI);
    // lua_pushvalue(internalstate, 2);
    int status = lua_pcall(internalstate, 0, 1, 0);
    if (status != 0) {
      noteyeError(9, "error running mainloopcyc", lua_tostring(internalstate, -1), status);
      }
    lua_pop(internalstate, 1);
    }
  }

int halfdelaymode = -1;
int cursorsize = 1;

void noteye_halfdelay(int i) { halfdelaymode = 100 * i; }
void noteye_halfdelayms(int i) { halfdelaymode = i; }
void noteye_cbreak() { halfdelaymode = -1; }
void noteye_curs_set(int i) { if(i==2) i=100; cursorsize = i; }
void noteye_curs_setx(int i) { cursorsize = i; }
void noteye_getclick(int &x, int &y, int &button) {
  if (P) {
    x = P->mouse.x;
    y = P->mouse.y;
    button = P->mouse.button;
    }
  else
    x = y = button = 0;
  }

SDL_Event *noteye_getevent() {
  if(P->lastevent) delete P->lastevent;
  P->lastevent = P->evbuf[P->evs];
  if(P->lastevent) {
    P->evbuf[P->evs] = NULL;
    P->evs = (1+P->evs) % EVENTBUFFER;
    }
  return P->lastevent;
  }

int noteye_eventtokey(SDL_Event *ev) {
  SDL_KeyboardEvent& kev(ev->key);

  switch(ev->type) {
    case SDL_TEXTINPUT:
      return kev.keysym.sym;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      SDL_MouseButtonEvent& mev(ev->button);
      P->mouse.x = mev.x;
      P->mouse.y = mev.y;
      P->mouse.button = mev.button;
      return KEY_MOUSE;
      }
    default:
      return 0;
    }

  bool down = ev->type == SDL_KEYDOWN;

  int retkey = 0;

  if(!down) return 0;

  int sym = kev.keysym.sym;
  
  #define Snd(key, x) else if(sym == SDLK_ ## key) retkey = x;
  #define SndKP(key, x) else if(sym == SDLK_KP_ ## key) retkey = x;
  
  if(sym == SDLK_LSHIFT) return 0;
  else if(sym == SDLK_RSHIFT) return 0;
  else if(sym == SDLK_LCTRL) return 0;
  else if(sym == SDLK_RCTRL) return 0;
  else if(sym == SDLK_LALT) return 0;
  else if(sym == SDLK_RALT) return 0;
  else if(sym == SDLK_CAPSLOCK) return 0;

  else if(sym == SDLK_RETURN) retkey = '\n';
  Snd(KP_ENTER, '\n')

  Snd(BACKSPACE, '\b')
  Snd(TAB, '\t')
  Snd(F1, KEY_F0+1)
  Snd(F2, KEY_F0+2)
  Snd(F3, KEY_F0+3)
  Snd(F4, KEY_F0+4)
  Snd(F5, KEY_F0+5)
  Snd(F6, KEY_F0+6)
  Snd(F7, KEY_F0+7)
  Snd(F8, KEY_F0+8)
  Snd(F9, KEY_F0+9)
  Snd(F10, KEY_F0+10)
  Snd(F11, KEY_F0+11)
  Snd(F12, KEY_F0+12)

  // linux terminal
  Snd(UP, D_UP)
  Snd(DOWN, D_DOWN)
  Snd(RIGHT, D_RIGHT)
  Snd(LEFT, D_LEFT)
  Snd(HOME, D_HOME)
  Snd(END, D_END)
  Snd(PAGEUP, D_PGUP)
  Snd(PAGEDOWN, D_PGDN)

  SndKP(8, D_UP)
  SndKP(2, D_DOWN)
  SndKP(6, D_RIGHT)
  SndKP(4, D_LEFT)
  SndKP(7, D_HOME)
  SndKP(1, D_END)
  SndKP(9, D_PGUP)
  SndKP(3, D_PGDN)
  SndKP(5, D_CTR)

  #undef Snd
  #undef SndKP
  
  if(sym > 0 && sym < 32) return sym;

  if(sym >= 'a' && sym <= 'z' && (ev->key.keysym.mod & (KMOD_LCTRL|KMOD_RCTRL))) 
    return sym - 96;

  // added by JT: maybe we need to do this differently, but it seems to work
  if(sym >= 'a' && sym <= 'z' && (ev->key.keysym.mod & KMOD_LALT)) 
    return sym;
  
  // use this for NotEye specials
  if((sym >= 1024 && sym < 0x10000000) || sym < 0)
    return sym;

  return retkey;
  }

int noteye_getchfull(bool total = false) {
  if(halfdelaymode >= 0) nextdelay = SDL_GetTicks() + halfdelaymode;
  else nextdelay = 0;
  
  if(!P) return NOTEYEERR;
  while(true) {
    if(uithread && uithread_err) 
      return NOTEYEERR;
    noteye_getevent();
    if(P->lastevent) {
      int key = noteye_eventtokey(P->lastevent);
      if(key || total) return key;
      }
    if(nextdelay && int(SDL_GetTicks()) >= nextdelay)
      return -1;
    noteye_refresh();
    }
  }

int noteye_getch() {
  return noteye_getchfull(false);
  }

int noteye_getchev() {
  return noteye_getchfull(true);
  }

SDL_Event *noteye_getlastkeyevent() {
  return P->lastevent;
  }

int noteye_lastkeyevent_type() { return P->lastevent->type; }

#ifdef SDL2
int noteye_lastkeyevent_mods() { return P->lastevent->key.keysym.mod; }
int noteye_lastkeyevent_symbol() { return P->lastevent->key.keysym.scancode; }
int noteye_lastkeyevent_chr() { return P->lastevent->key.keysym.sym; }

#else
int noteye_lastkeyevent_mods() { return P->lastevent->key.keysym.mod; }
int noteye_lastkeyevent_symbol() { return P->lastevent->key.keysym.sym; }
int noteye_lastkeyevent_chr() { return P->lastevent->key.keysym.unicode; }
#endif

int noteye_inch() {
  if(P)
    return getChar(P->s->get(P->curx, P->cury));
  return 0;
  }

void noteye_addchx(int ch) {
  if(!P) return;
  P->changed = true;
  if(ch == '\n') {
    P->curx = 0;
    if(P->cury < P->s->sy-1) P->cury++;
    return;
    }
  if(P->curx < P->s->sx)
    P->s->get(P->curx, P->cury) = 
        addMerge(
          P->brushback,
          addRecolor(P->f->gettile(ch), P->fore, recDefault),
          false
          );
  P->curx++;
  if (P->curx >= P->s->sx) {
    P->curx = 0;
    if(P->cury < P->s->sy-1) P->cury++;
  }
  }

void noteye_addch(char ch) {
  noteye_addchx(ch & 0xFF);
  }

void noteye_addstr(const char *buf) { 
  while(*buf) noteye_addch(*buf), buf++;
  }

void noteye_endwin() { }

void noteye_erase() { 
  if(!P) return;
  P->changed = true;
  for(int y=0; y<P->s->sy; y++) for(int x=0; x<P->s->sx; x++) 
    P->s->get(x, y) = P->brush0;
  P->curx=0; P->cury=0;
  }
void noteye_move(int y, int x) {
  if(!P) return;
  P->changed = true;
  P->curx=x; P->cury=y;
  if(P->curx<0) P->curx=0;
  if(P->curx>=P->s->sx) P->curx=P->s->sx-1;
  if(P->cury<0) P->cury=0;
  if(P->cury>=P->s->sy) P->cury=P->s->sy-1;
  }
void noteye_clrtoeol() { 
  if(!P) return;
  P->changed = true;
  if(P->cury<P->s->sy)
  for(int x=P->curx; x<P->s->sx; x++)
    P->s->get(x, P->cury) = P->brush0;
  }

void noteye_mvaddch(int y, int x, char ch) {
  noteye_move(y, x); noteye_addch(ch);
  }

int noteye_mvinch(int y, int x) {
  noteye_move(y, x); return noteye_inch();
  }

void noteye_mvaddchx(int y, int x, int ch) {
  noteye_move(y, x); noteye_addchx(ch);
  }

void noteye_mvaddstr(int y, int x, const char *buf) {
  noteye_move(y,x); noteye_addstr(buf);
  }

void setTextAttr32(int fore, int back) {
  if(P) P->setColor(fore, back);
  }

void setTextAttr(int fore, int back) {
  setTextAttr32(vgacol[fore & 15], vgacol[back & 15]);
  }

int getVGAcolor(int c) {
  return vgacol[c & 15];
  }
}

#undef P


// ---

InternalProcess::InternalProcess(Screen *scr, Font *f, const char *cmdline) : Process(scr, f, cmdline) {

  isActive = true;  changed = false;
  curx = 0; cury = 0; 
  setColor(vgacol[7], vgacol[0]);
  evs = 0; eve = 0; lastevent = NULL;
  for(int i=0; i<EVENTBUFFER; i++) evbuf[i] = NULL;
  
  for(int x=0; x<scr->sx; x++) for(int y=0; y<scr->sy; y++)
    scr->get(x,y) = brush0;
  }

InternalProcess::~InternalProcess() {
  for(int i=0; i<EVENTBUFFER; i++) if(evbuf[i]) delete evbuf[i];  
  }

void InternalProcess::sendText(const string& s) {
  for(int i=0; i<size(s); i++) {
    int neve = (eve + 1) % EVENTBUFFER;
    if(neve != evs) {
      SDL_Event *ev = new SDL_Event;
      ev->type = SDL_TEXTINPUT;
      ev->key.keysym.sym = s[i];
      ev->key.keysym.mod = (i == size(s) - 1) ? 1 : 0;
      ev->key.keysym.scancode = SDL_Scancode(size(s) * 256 + i);
      evbuf[eve] = ev;
      eve = neve;
      }
    }
  }

void InternalProcess::sendKey(int scancode, int keycode, int mod, bool down) {
  int neve = (eve + 1) % EVENTBUFFER;
  if(neve != evs) {
    SDL_Event *ev = new SDL_Event;
    ev->type = down ? SDL_KEYDOWN : SDL_KEYUP;
    SDL_KeyboardEvent& kev(ev->key);

    kev.keysym.sym = keycode;
    kev.keysym.mod = mod;
    kev.keysym.scancode = SDL_Scancode(scancode);

    evbuf[eve] = ev;
    eve = neve;
    // todo: free them if unused
    }
  else {
    // todo: currently we simply drop the extra keypresses,
    // we should probably do something in this case!
    }

  }

void InternalProcess::sendClick(int x, int y, int button) {
  int neve = (eve + 1) % EVENTBUFFER;
  if(neve != evs) {
    SDL_Event *ev = new SDL_Event;
    ev->type = SDL_MOUSEBUTTONDOWN;
    SDL_MouseButtonEvent& mev(ev->button);
    mev.button = button;
    mev.state = SDL_PRESSED;
    mev.x = x;
    mev.y = y;
    evbuf[eve] = ev;
    eve = neve;
    }
  }

bool InternalProcess::checkEvent(lua_State *L) {

  if(!isActive) {
    lua_newtable(L);
    noteye_table_setInt(L, "type", evProcQuit);
    noteye_table_setInt(L, "obj", P->id);
    noteye_table_setInt(L, "exitcode", exitcode);
    return true;
    }

  if(changed) {
    changed = false;
    lua_newtable(L);
    noteye_table_setInt(L, "type", evProcScreen);
    noteye_table_setInt(L, "obj", id);
    return true;
    }
  
  return false;
  }

void InternalProcess::setColor(int _fore, int _back) {
  // brush.back = vgacol[ mtrans[back] ];
  back = _back;
  fore = _fore;
  brushback = addFill(back, 0xffffff);
  int rec = addRecolor(f->gettile(32), fore, 0xffffff);
  brush0 = addMerge(brushback, rec, false);
  }

int InternalProcess::getCursorSize() {
  return cursorsize;
  }

#ifdef USELUA

extern "C" {

int lh_internal(lua_State *L) {
  checkArg(L, 3, "internal");

  P = new InternalProcess(luaO(1, Screen), luaO(2, Font), luaStr(3));
  
  return retObjectEv(L, P);
  }

}

#endif

extern "C" {

void noteye_setinternal(InternalProcess *Proc, lua_State *L, int spos) {
  P = Proc;
  internalstate = L;
  lua_pushvalue(L, spos);
  lua_setglobal(L, CALLUI);
  }

InternalProcess *noteye_getinternal() {
  return P;
  }

int noteye_getinternalx() { return P->s->sx; }
int noteye_getinternaly() { return P->s->sy; }

void noteye_finishinternal(int exitcode) {
  P->exitcode = exitcode;
  P->isActive = false;
  }

int lh_uicreate(lua_State *L) {
  // kill the old thread?
  lua_setglobal(L, "threadtemp");
  uithread = lua_newthread(L);
  uithread_out = L;
  lua_getglobal(uithread, "threadtemp");
  uithread_running = true;
  int status = lua_resume(uithread, 0);
  luamapstate = uithread_out;
  uithread_err = false;
  if (status != LUA_YIELD) {
    noteyeError(10, "error running uicreate", lua_tostring(uithread, -1), status);
    uithread_err = true;
    }
  uithread_running = false;
  return 1;
  }

int lh_uisleep(lua_State *L) {
  return lua_yield(L,0);
  }

void noteye_uiresume() {
  if(!uithread) {
    noteyeError(11, "no UI thread to resume", NULL);
    return;
    }
  uithread_running = true;
  int status = lua_resume(uithread, 0);
  luamapstate = uithread_out;
  uithread_running = false;
  if(status != LUA_YIELD) {
    noteyeError(14, "uiresume did not yield", lua_tostring(uithread, -1), status);
    uithread_err = true;
    }
  }

void noteye_uifinish() {
  if(!uithread) {
    noteyeError(13, "no UI thread to finish", NULL);
    return;
    }
  uithread_running = true;
  int t = 100;
  while(true) {
    int status = lua_resume(uithread, 0);
    if(status == 0) break;
    if(status == LUA_YIELD && t>0) { t--; continue; }
    noteyeError(12, "uifinish did not finish thread", lua_tostring(uithread, -1), status);
    }
  uithread_running = false;
  uithread = NULL;
  }

}
}
