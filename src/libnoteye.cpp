// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

#define USELUA

#include "noteye.h"

#ifdef USELUA
extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
  }
#endif

using namespace std;

#include "util.cpp"
#include "opengl.cpp"
#include "sdltex.cpp"
#include "gfx.cpp"
#include "image.cpp"
#include "tiles.cpp"
#include "drawtile.cpp"
#include "screen.cpp"
#include "fpp.cpp"
#include "iso.cpp"
#include "font.cpp"
#include "process.cpp"
#include "stream.cpp"
#include "consout.cpp"
#include "sound.cpp"
#include "lua.cpp"

using namespace noteye;

extern "C" {

void noteye_init() {
  errfile = stdout;
  objs.resize(0);
  objs.push_back(NULL);
  eventobjs.resize(0);
  eventobjs.push_back(1);
  initMode();  
  initMappings();
  initLua();
  }

void noteye_run(const char *noemain, bool applyenv) {
  
  char *buf;
  
  if(applyenv && getenv("NOTEYEDIR")) {
    buf = (char*) malloc(strlen(getenv("NOTEYEDIR")) + strlen(noemain) + 8);
    sprintf(buf, "%s/%s", getenv("NOTEYEDIR"), noemain);
    }
  else buf = strdup(noemain);
  
  if(luaL_dofile(LS, buf))
    noteyeError(21, "dofile", lua_tostring(LS, -1));
  
  free(buf);
  }

void noteye_handleerror(noteyehandler h) {
  noteyeErrorHandler = h;
  }

void noteye_halt() {
  closeLua();  
  if(logfile) {
    fprintf(logfile, "%s", noteyeStats());
    fclose(logfile);
    logfile = NULL;
    }
#ifndef NOAUDIO
  extern void closeAudio();
  closeAudio();
#endif
  initJoysticks(false);
  for(int i=1; i<size(objs); i++) deleteobj(i);
  deleted_objects.clear();
  SDL_Quit();
  SDL_FreeSurface(exsurface); exsurface = NULL;
  for(int i=0; i<=HASHMAX; i++) if(hashtab[i] != NULL) {
    printf("hashtab not clear\n");
    hashtab[i] = NULL;
    }
  P = NULL;
  }

}
