// Necklace of the Eye v5.1
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

// an alternative Libtcod support implementation: as a libnoteye user.
// The old implementation (libtcod.cpp) is still preferred, since
// it seems simpler to use in most cases. 
// (download http://roguetemple.com/z/noteye-libtcod-user.zip for the rest
// of files needed to compile this)

#include "noteye.h"

extern "C" {

static Font   *fnt;
static Screen *scr;

TCOD_noteye tnoteye;

bool quitit = false;

int lh_quitlibtcod(lua_State *L) {
  quitit = true;
  return 0;
  }

int lh_tcodmouse(lua_State *L) {
  checkArg(L, 3, "tcodmouse");
  tnoteye.x = luaInt(1);
  tnoteye.y = luaInt(2);
  tnoteye.state = luaInt(3);
  tnoteye.active = true;
  return 0;
  }

void TCOD_noteye_init() {

  if(scr) return;
  tnoteye.mode = 3;
  
  noteye_args(0, NULL);
  noteye_init();
  
  if(!getenv("NOTEYENAME") || !getenv("NOTEYEDIR")) {
    printf("Please set up NOTEYENAME and NOTEYEDIR!\n");
    exit(1);
    }

  noteye_globalstr("libtcodname", getenv("NOTEYENAME"));
  noteye_globalfun("quitlibtcod", lh_quitlibtcod);
  noteye_globalfun("tcodmouse", lh_tcodmouse);

  const char *scri = getenv("NOTEYESCRIPT");
  if(!scri) scri = "common/from-libtcod.noe";
  noteye_run(scri, true);
  
  InternalProcess *P = noteye_getinternal();
  scr = P->s;
  }

char_t *tbuffer;

static int noteyecolor(const TCOD_color_t& color) {
  int i = 0;
  i += (color.r) << 16;
  i += (color.g) << 8;
  i += (color.b) << 0;
  return i;
  }

static void TCOD_noteye_render() {
  TCOD_noteye_init();
  int sx = TCOD_console_get_width(NULL);
  int sy = TCOD_console_get_height(NULL);

  InternalProcess *P = noteye_getinternal();
  scr = P->s;
  fnt = P->f;

  scr->setSize(sx, sy);
  char_t *buffer = tbuffer;
  for(int y=0; y<sy; y++) for(int x=0; x<sx; x++) {
    int& I(scr->get(x,y));
    int J = 
      addMerge(
//      addFill(0), addRecolor(fnt->ti[buffer->c], 0xFFFFFF)
        addFill(noteyecolor(buffer->back), 0xFFFFFF), addRecolor(fnt->ti[buffer->c], noteyecolor(buffer->fore), recDefault),
        false
        );
    if(I != J) P->changed = true;
    I = J;
    buffer++;
    }

  TileImage *TI= dbyId<TileImage> (fnt->ti[32]);
  if(TI) {
    TCOD_ctx.font_width = TI->sx;
    TCOD_ctx.font_height = TI->sy;
    }
  }

void TCOD_noteye_sendscreen(char_t *buffer) {
  tbuffer = buffer;
  TCOD_noteye_init();
  TCOD_noteye_render();
  }

void TCOD_noteye_check() {
  TCOD_noteye_init();
  noteye_uiresume();
  }

SDL_Event *TCOD_noteye_event() {
  if(quitit) {
    SDL_Event *ev = new SDL_Event;
    ev->type = SDL_QUIT;
    quitit = false;
    return ev;
    }
  TCOD_noteye_init();
  InternalProcess *P = noteye_getinternal();
  if(P->evs == P->eve) return NULL;

  SDL_Event *ret = P->evbuf[P->evs];
  P->evs = (1+P->evs) % EVENTBUFFER;
  return ret;
  }

}