// Necklace of the Eye v3.0
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#include "internal.cpp"

#if INTERNALONLY
#else
#ifdef LINUX
#include "linux.cpp"
#else
#include "windows.cpp"
#endif
#endif

namespace noteye {

extern "C" {

#ifndef INTERNALONLY
Process *newProcess(Screen *s, Font *f, const char *str) {
  Process *p = startProcess(s, f, str);
  add_event_listener(registerObject(p));  
  return p;
  }
#endif

point noteye_getcursor(Process *P) {
  point res;
  if(!P) { res.x = 0; res.y = 0; return res; }
  res.x = P->curx;
  res.y = P->cury;
  return res;
  }

int noteye_getcursorsize(Process *P) {
  return P->getCursorSize();
  }

void sendkey(Process *P, int scancode, int keycode, int mod, int type) {
  
  if(type != evKeyDown && type != evKeyUp)
    printf("WARNING: sending a key with wrong type\n");

  P->sendKey(scancode, keycode, mod, type == evKeyDown);
  }

void sendtext(Process *P, const char *s) {
  P->sendText(s);
  }

int sendclick(Process *P, int x, int y, int button) {
  P->sendClick(x, y, button);
  return 0;
  }

bool processActive(Process *P) {
  return P->active();
  }

}

}
