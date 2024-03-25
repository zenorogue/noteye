// Hydra Slayer roguelike
// Copyright (C) 2010-2015 Zeno Rogue
// NotEye interface

extern int hydraMain(int argc, char ** argv);
extern void drawMapLua(lua_State *L, int x, int y, int m);
extern void getcoordsLua(lua_State *L);
extern void cacheMap(int id, int val);
extern std::string helpAbout(int x, int y, bool lng);
extern void queueTarget(int x, int y);
extern void hydraUserdir(const std::string& userdir);
extern void shareUpdate();

extern int hydranoteyeflag;

extern int lh_getSounds(lua_State *L);

#define luaO(x,t) dynamic_cast<t*> (noteye_by_handle(luaInt(x)))

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

int lh_getcoords(lua_State *L) {
  checkArg(L, 0, "getcoords");
  getcoordsLua(L);
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
  checkArg(L, 2, "hydrainfo");
  int x = luaInt(1);
  int y = luaInt(2);
  helpAbout(x, y, true);
  return 0;
  }

int lh_hydralook(lua_State *L) {
  checkArg(L, 2, "hydralook");
  int x = luaInt(1);
  int y = luaInt(2);
  static std::string looks;
  looks = helpAbout(x, y, false);
  return noteye_retStr(L, looks.c_str());
  }

int lh_queuetarget(lua_State *L) {
  checkArg(L, 2, "hydramap");
  int x = luaInt(1);
  int y = luaInt(2);
  queueTarget(x, y);
  return 0;
  }

void prepareheads();

int lh_sethydrabox(lua_State *L) {
  checkArg(L, 3, "sethydrabox");
  extern SDL_Surface *hydrabox, *hydraheads, *hydraheads2;
  extern int *hydraboxc;
  extern Image *headimage;
  Image *I1 = luaO(1, Image);
  Image *I2 = luaO(2, Image);
  Image *I3 = luaO(3, Image);
  headimage = I3;
  hydrabox = I1->s;
  hydraheads = I2->s;
  hydraheads2 = I3->s;
  hydraboxc = &I1->changes;
  prepareheads();
  return 0;
  }

int lh_animatehydras(lua_State *L) {
  checkArg(L, 1, "animatehydras");
  extern void hydrapicAnimate(int);
  hydrapicAnimate(luaInt(1));
  return 0;
  }

int lh_hydrasetflag(lua_State *L) {
  checkArg(L, 1, "hydrasetflag");
  hydranoteyeflag = luaInt(1);
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
  noteye_globalfun("hydralook", lh_hydralook);
  noteye_globalfun("hydraqueuetarget", lh_queuetarget);
  noteye_globalfun("hydramain", lh_hydramain);
  noteye_globalfun("hydrauserdir", lh_hydrauserdir);
  noteye_globalfun("hydrasounds", lh_getSounds);
  noteye_globalfun("hydracoords", lh_getcoords);
  noteye_globalfun("sethydrabox", lh_sethydrabox);
  noteye_globalfun("animatehydras", lh_animatehydras);
  noteye_globalfun("shareupdate", lh_shareupdate);
  noteye_globalfun("hydrasetflag", lh_hydrasetflag);
  shareUpdate();
  }

