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

#ifdef USELUA

#ifndef INTERNALONLY
int lh_newProcess(lua_State *L) {
  checkArg(L, 3, "newprocess");
  Process *p = startProcess(luaO(1, Screen), luaO(2, Font), luaStr(3));
  return retObjectEv(L, p);
  }
#endif

int lh_proccur(lua_State *L) {
  checkArg(L, 1, "proccur");
  Process *P = luaO(1, Process);
  lua_newtable(L);
  noteye_table_setInt(L, "x", P->curx);
  noteye_table_setInt(L, "y", P->cury);
  noteye_table_setInt(L, "size", P->getCursorSize());
  return 1;
  }

int lh_sendkey(lua_State *L) {
  checkArg(L, 2, "sendkey");
  Process *P = luaO(1, Process);

  int scancode = getfieldInt(L, "scancode");
  int keycode = getfieldInt(L, "keycode");
  int mod = getfieldInt(L, "mod");
  int type = getfieldInt(L, "type");
  
  if(type != evKeyDown && type != evKeyUp)
    printf("WARNING: sending a key with wrong type\n");

  P->sendKey(scancode, keycode, mod, type == evKeyDown);
  return 0;
  }

int lh_sendtext(lua_State *L) {
  checkArg(L, 2, "sendtext");
  Process *P = luaO(1, Process);

  P->sendText(luaStr(2));
  return 0;
  }

int lh_sendclick(lua_State *L) {
  checkArg(L, 4, "sendclick");
  Process *P = luaO(1, Process);
  int x = luaInt(2);
  int y = luaInt(3);
  int button = luaInt(4);
  P->sendClick(x, y, button);
  return 0;
  }

int lh_processActive(lua_State *L) {
  checkArg(L, 1, "processactive");
  return noteye_retInt(L, luaO(1, Process)->active());
  }

#endif

}
