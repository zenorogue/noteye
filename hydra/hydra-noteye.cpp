// Hydra Slayer roguelike
// Copyright (C) 2010-2015 Zeno Rogue
// NotEye interface

extern int hydraMain(int argc, char ** argv);
extern void drawMapLua(lua_State *L, int x, int y, int m);
extern void cacheMap(int id, int val);
extern void helpAbout(int x, int y);
extern void hydraUserdir(const std::string& userdir);
extern void shareUpdate();

extern int lh_getSounds(lua_State *L);

int lh_hydramain(lua_State *L) {
  checkArg(L, 2, "hydramain");

  InternalProcess *P = luaO(1, InternalProcess);
  noteye_setinternal(P, L, 2);

  P->exitcode = hydraMain(gargc, gargv);
  P->isActive = false;

  return 0;
  }

int lh_hydrauserdir(lua_State *L) {
  checkArg(L, 1, "hydrauserdir");
  hydraUserdir(luaStr(1));
  return 0;
  }

int lh_hydramap(lua_State *L) {
  checkArg(L, 3, "hydramap");
  int x = luaInt(1);
  int y = luaInt(2);
  int m = luaInt(3);
  drawMapLua(L, x, y, m);
  return 1;
  }

int lh_hydracache(lua_State *L) {
  checkArg(L, 2, "hydracache");
  int id = luaInt(1);
  int val = luaInt(2);
  cacheMap(id, val);
  return 1;
  }

extern int DIRS;

int lh_hydraDIRS(lua_State *L) {
  checkArg(L, 0, "hydraDIRS");
  return noteye_retInt(L, DIRS);
  }

int lh_hydrainfo(lua_State *L) {
  checkArg(L, 2, "hydramap");
  int x = luaInt(1);
  int y = luaInt(2);
  helpAbout(x, y);
  return 0;
  }

int lh_sethydrabox(lua_State *L) {
  checkArg(L, 2, "sethydrabox");
  extern SDL_Surface *hydrabox, *hydraheads;
  extern int *hydraboxc;
  Image *I1 = luaO(1, Image);
  Image *I2 = luaO(2, Image);
  hydrabox = I1->s;
  hydraheads = I2->s;
  hydraboxc = &I1->changes;
  return 0;
  }

int lh_animatehydras(lua_State *L) {
  checkArg(L, 1, "animatehydras");
  extern void hydrapicAnimate(int);
  hydrapicAnimate(luaInt(1));
  return 0;
  }

int lh_shareupdate(lua_State *L) {
  checkArg(L, 0, "shareupdate");
  shareUpdate();
  return 0;
  }

void addHydraToLua() {
  extern const char* hydraver;
  noteye_globalstr("hydraver", hydraver);
  noteye_globalfun("hydramap", lh_hydramap);
  noteye_globalfun("hydracache", lh_hydracache);
  noteye_globalfun("hydraDIRS", lh_hydraDIRS);
  noteye_globalfun("hydrainfo", lh_hydrainfo);
  noteye_globalfun("hydramain", lh_hydramain);
  noteye_globalfun("hydrauserdir", lh_hydrauserdir);
  noteye_globalfun("hydrasounds", lh_getSounds);
  noteye_globalfun("sethydrabox", lh_sethydrabox);
  noteye_globalfun("animatehydras", lh_animatehydras);
  noteye_globalfun("shareupdate", lh_shareupdate);
  }
