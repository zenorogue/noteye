// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#include "noteye.h"

using namespace noteye;

// #define HYDRA // undefine if you do not want Hydra Slayer included internally

static int gargc;
static char **gargv;

#ifdef HYDRA
extern int hydraMain(int argc, char ** argv);
extern int drawMapLua(lua_State *L, int x, int y);
extern void helpAbout(int x, int y);

extern int lh_getSounds(lua_State *L);

int lh_hydramain(lua_State *L) {
  checkArg(L, 2, "hydramain");

  InternalProcess *P = luaO(1, InternalProcess);
  noteye_setinternal(P, L, 2);
  P->exitcode = hydraMain(gargc, gargv);
  P->isActive = false;

  return 0;
  }

int lh_hydramap(lua_State *L) {
  checkArg(L, 2, "hydramap");
  int x = luaInt(1);
  int y = luaInt(2);
  drawMapLua(L, x, y);
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

#endif

#undef main

#ifdef MAC
int SDL_main(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif

  noteye_args(gargc=argc, gargv=argv);
  
  noteye_init();

#ifdef HYDRA
  extern const char* hydraver;
  noteye_globalstr("hydraver", hydraver);
  noteye_globalfun("hydramap", lh_hydramap);
  noteye_globalfun("hydraDIRS", lh_hydraDIRS);
  noteye_globalfun("hydrainfo", lh_hydrainfo);
  noteye_globalfun("hydramain", lh_hydramain);
  noteye_globalfun("hydrasounds", lh_getSounds);
  noteye_globalfun("sethydrabox", lh_sethydrabox);
  noteye_globalfun("animatehydras", lh_animatehydras);
#endif

#ifdef HYDRAONLY  
  noteye_run("common/hydraslayer.noe", true);
#else
  noteye_run("common/noteye.noe", true);
#endif
  noteye_halt();
  return 0;
  }

