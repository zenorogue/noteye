// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

// on Mac and other architectures, use a temporary buffer
#ifdef MAC
#define USEGFXBUFFER
#endif

namespace noteye {

static SDL_Surface *exsurface;

#ifndef LIBTCOD
static bool sdlerror = false;
#endif

vector<point> origs;

SDL_Joystick *joysticks[8];

bool isjoyon;

void initJoysticks(bool joyon) {
  if(joyon == isjoyon) return;
  isjoyon = joyon;
  if(joyon) {
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    
    int nj = SDL_NumJoysticks();
    for(int i=0; i<8; i++)  
      if(i < nj) joysticks[i] = SDL_JoystickOpen(i);
    }
  
  else {
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    for(int i=0; i<8; i++) 
      if(joysticks[i]) {
        SDL_JoystickClose(joysticks[i]);
        joysticks[i] = NULL;
        }
    }
  }

#ifndef LIBTCOD
void initMode() {

  if(sdlerror) return;
  
  if(exsurface) return;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(errfile, "Failed to initialize SDL: '%s'\n", SDL_GetError());
    sdlerror = true;
    return;
    }
  
  // IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

  origs.clear();
  
#ifdef SDL2
  int displays = SDL_GetNumVideoDisplays();
  for(int i=0; i<displays; i++) {
    SDL_DisplayMode curr;
    SDL_GetCurrentDisplayMode(i, &curr);
    point p = {curr.w, curr.h};
    origs.push_back(p);
    }

#else
  const SDL_VideoInfo *inf = SDL_GetVideoInfo();
  point p = {inf->current_w, inf->current_h};
  origs.push_back(p);

#endif


#ifndef SDL2
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_EnableUNICODE(1);
#endif

  SDL_StartTextInput();

  exsurface=SDL_CreateRGBSurface(SDL_SWSURFACE,16,16,32,0,0,0,0);
  if(exsurface == NULL) {
    fprintf(errfile, "CreateRGBSurface failed: %s\n", SDL_GetError());
    sdlerror = true;
    return;
    }
  }
#endif  

#ifdef USELUA
int lh_findvideomode(lua_State *L) {
  int sx = luaInt(1);
  int sy = luaInt(2);
  
  initMode();
  
  int nsx = 99999, nsy = 99999;

  int nummodes = SDL_GetNumDisplayModes(0);
  if(nummodes == 0) nsx = sx, nsy = sy;
  else for(int i=0; i<nummodes; i++) {
    SDL_DisplayMode modei;
    SDL_GetDisplayMode(0, i, &modei);
    if (modei.w >= sx && modei.h >= sy) {
      if(modei.w > nsx || modei.h > nsy) continue;
      // printf("  chosen!\n");
      nsx = modei.w;
      nsy = modei.h;
      }
    }
  
  lua_newtable(L);
  noteye_table_setInt(L, "x", nsx);
  noteye_table_setInt(L, "y", nsy);
  return 1;
  }

int lh_origvideomode(lua_State *L) {
  
  int displayid = 0;
  
  if(noteye_argcount(L) == 1) {

    Window *w = dluaO(1, Window);
    displayid = w ? SDL_GetWindowDisplayIndex(w->win) : SDL_GetWindowDisplayIndex(NULL);
    }
  
  lua_newtable(L);
  noteye_table_setInt(L, "x", origs[displayid].x);
  noteye_table_setInt(L, "y", origs[displayid].y);
  return 1;
  }

#ifdef USEGFXBUFFER
SDL_Surface *gfxbuffer;
SDL_Surface *gfxreal;
#endif

bool hadGL = false;

void Window::close() {
  if(flags & SDL_WINDOW_OPENGL) disableGL(this);
  else if(usetex) disableSDL(this);
  if(s) SDL_FreeSurface(s);
  s = NULL;
  if(tex) SDL_DestroyTexture(tex);
  tex = NULL;
  if(ren) SDL_DestroyRenderer(ren);
  ren = NULL;
  if(win) SDL_DestroyWindow(win);
  win = NULL;
  }

bool Window::useSurfaceSize(int x, int y) {
  if(tex) {
    int sx, sy;
    SDL_QueryTexture(tex, NULL, NULL, &sx, &sy);
    if(sx != x || sy != y) {
      SDL_DestroyTexture(tex);
      tex = NULL;
      }
    }
  if(!tex) {
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, x, y);
    if(!tex) return false;
    }
  if(s && (s->w != x || s->h != y)) {
    SDL_FreeSurface(s);
    s = NULL;
    }
  if(!s) {
    s = SDL_CreateRGBSurface(0, x, y, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if(!s) return false;
    }
  return true;
  }

bool Window::open(int x, int y, int newflags, int newrenflags, int px, int py) {

  if(win && (flags ^ SDL_WINDOW_FULLSCREEN) == newflags && x == sx && y == sy) {
    SDL_SetWindowFullscreen(win, newflags & SDL_WINDOW_FULLSCREEN);
    flags ^= SDL_WINDOW_FULLSCREEN;
    }

  if(win && flags != newflags) {
    close();
    }
  
  if(win && (x != sx || y != sy)) {
    sx = x; sy = y;
    SDL_SetWindowSize(win, x, y); // to do check
#ifdef OPENGL
    if(flags & SDL_WINDOW_OPENGL) {
      printf("doing initOrthoGL\n");
      initOrthoGL(this);
      }
#endif
    }

  if(win && (px != SDL_WINDOWPOS_UNDEFINED || py != SDL_WINDOWPOS_UNDEFINED)) {
    SDL_SetWindowPosition(win, px, py); // to do check
    }

  if(!win) {
    flags = newflags; sx = x; sy = y;
    
    win = SDL_CreateWindow(title.c_str(), px, py, sx, sy, flags);
    if(!win) return false;
  
  #ifdef OPENGL
    if(flags & SDL_WINDOW_OPENGL) {
      initOrthoGL(this);
      }
  #endif
    }

  if(ren && renflags != newrenflags) {
    if(usetex) disableSDL(this);
    SDL_DestroyTexture(tex);
    tex = NULL;
    SDL_DestroyRenderer(ren);
    ren = NULL;
    }
  
  if(!ren && !(flags & SDL_WINDOW_OPENGL)) {
    ren = SDL_CreateRenderer(win, -1, renflags = newrenflags);
    if(!ren) return false;
    /* Draw a black screen */
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);
    }
  
  #ifdef OPENGL
  if(flags & SDL_WINDOW_OPENGL) {
    renflags = newrenflags;
    int i = (renflags & SDL_RENDERER_PRESENTVSYNC) ? 1 : 0;
    SDL_GL_SetSwapInterval(i);
    }
  #endif
  
  if(!usetex && !(flags & SDL_WINDOW_OPENGL)) {
    if(!useSurfaceSize(x, y)) return false;
    }

  return true;
  }

int lh_newwindow(lua_State *L) {
  checkArg(L, 1, "newwindow");
  Window *s = new Window;
  s->win = NULL;
  s->ren = NULL;
  s->s = NULL;
  s->tex = NULL;
  s->gl = NULL;
  s->usetex = false;
  s->title = luaStr(1);

  return noteye_retObject(L, s);
  }

int lh_openwindow(lua_State *L) {
  checkArg(L, 7, "openwindow");

  return noteye_retBool(L, 
    luaO(1, Window)->open(luaInt(2), luaInt(3), luaInt(4), luaInt(5), luaInt(6), luaInt(7))
    );  
  }

int lh_windowusetex(lua_State *L) {
  checkArg(L, 2, "windowusetex");

  Window *w = luaO(1, Window);
  
  w->usetex = luaBool(2);
  
  if(!w->usetex) {
    disableSDL(w);
    if(!w->tex) {
      w->tex = SDL_CreateTexture(w->ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w->sx, w->sy);
      if(!w->tex) return false;
      }
    if(!w->s) {
      SDL_FreeSurface(w->s);
      w->s = SDL_CreateRGBSurface(0, w->sx, w->sy, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
      if(!w->s) return false;
      }
    }
    
  return 0;
  }

int lh_SDL_GetRendererInfoName(lua_State *L) {
  checkArg(L, 1, "SDL_GetRendererInfoName");
  Window *w = luaO(1, Window);
  
  if(w->flags & SDL_WINDOW_OPENGL)
    return noteye_retStr(L, "(NotEye's OpenGL)");

  SDL_RendererInfo info;
    
  SDL_GetRendererInfo(w->ren, &info);
  
  return noteye_retStr(L, info.name);
  }

int lh_closewindow(lua_State *L) {
  checkArg(L, 1, "closewindow");

  luaO(1, Window)->close();
  return 0;
  }

int lh_setwindowtitle(lua_State *L) {
  checkArg(L, 2, "setwindowtitle");
  Window *w = luaO(1, Window);
  w->title = luaStr(2);
  if(w->win)
    SDL_SetWindowTitle(w->win, w->title.c_str());
  return 0;
  }

int lh_setwindowminsize(lua_State *L) {
  checkArg(L, 3, "setwindowminsize");
  Window *w = luaO(1, Window);
  if(w->win)
    SDL_SetWindowMinimumSize(w->win, luaInt(2), luaInt(3));
  return 0;
  }

int lh_setwindowmaxsize(lua_State *L) {
  checkArg(L, 3, "setwindowmaxsize");
  Window *w = luaO(1, Window);
  if(w->win)
    SDL_SetWindowMaximumSize(w->win, luaInt(2), luaInt(3));
  return 0;
  }

int lh_setwindowicon(lua_State *L) {
  checkArg(L, 2, "setwindowicon");
  Window *w = luaO(1, Window);
  Image *i = luaO(2, Image);
  if(w->win)
    SDL_SetWindowIcon(w->win, i->s);
  return 0;
  }

int lh_renderwindow(lua_State *L) {
  checkArg(L, 1, "renderwindow");
  Window *w = luaO(1, Window);
  
#ifdef OPENGL
  if(useGL(w)) {
    refreshGL(w);
    return 0;
    }
#endif

  if(useSDL(w)) {
    refreshSDL(w);
    return 0;
    }

  SDL_UpdateTexture(w->tex, NULL, w->s->pixels, w->s->pitch);
  SDL_RenderClear(w->ren);
  SDL_RenderCopy(w->ren, w->tex, NULL, NULL);
  SDL_RenderPresent(w->ren);

  return 0;
  }

int lh_enablejoysticks(lua_State *L) {
  checkArg(L, 1, "enablejoysticks");
  initJoysticks(luaBool(1));
  return 0;
  }

int lh_enablekeyrepeat(lua_State *L) {
  checkArg(L, 2, "enablekeyrepeat");
  
#ifndef SDL2
  int delay = luaInt(1);
  int interval = luaInt(2);

  if(delay == -1) delay = SDL_DEFAULT_REPEAT_DELAY;
  if(interval == -1) interval = SDL_DEFAULT_REPEAT_INTERVAL;
  
  SDL_EnableKeyRepeat(delay, interval);
#endif

  return 0;
  }

int lh_SDL_GetKeyFromName(lua_State *L) {
  checkArg(L, 1, "SDL_GetKeyFromName");
  
  return noteye_retInt(L, SDL_GetKeyFromName(luaStr(1)));
  }

int lh_SDL_GetScancodeFromName(lua_State *L) {
  checkArg(L, 1, "SDL_GetScancodeFromName");
  
  return noteye_retInt(L, SDL_GetScancodeFromName(luaStr(1)));
  }

int lh_SDL_GetKeyName(lua_State *L) {
  checkArg(L, 1, "SDL_GetKeyName");
  
  return noteye_retStr(L, SDL_GetKeyName(luaInt(1)));
  }

int lh_SDL_GetScancodeName(lua_State *L) {
  checkArg(L, 1, "SDL_GetScancodeName");
  
  return noteye_retStr(L, SDL_GetScancodeName((SDL_Scancode) luaInt(1)));
  }

int lh_SDL_GetKeyFromScancode(lua_State *L) {
  checkArg(L, 1, "SDL_GetKeyFromScancode");
  
  return noteye_retInt(L, SDL_GetKeyFromScancode((SDL_Scancode) luaInt(1)));
  }

int lh_SDL_GetScancodeFromKey(lua_State *L) {
  checkArg(L, 1, "SDL_GetScancodeFromKey");
  
  return noteye_retInt(L, SDL_GetScancodeFromKey(luaInt(1)));
  }

int lh_SDL_ShowCursor(lua_State *L) {
  checkArg(L, 1, "SDL_ShowCursor");
  return noteye_retInt(L, SDL_ShowCursor(luaInt(1)));
  }
#endif

int nextdelay = 0;

#ifndef LIBTCOD
bool checkEventSDL(lua_State *L, int timeout) {
  initMode();
  if(sdlerror) return false;
  fflush(logfile);
  SDL_Event ev;
  if(nextdelay) timeout = min(timeout, nextdelay - (int) SDL_GetTicks());
  if(timeout < 0) timeout = 0;
  while(timeout ? SDL_WaitEventTimeout(&ev, timeout) : SDL_PollEvent(&ev)) {

    if(ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
      SDL_KeyboardEvent& kev(ev.key);
      lua_newtable(L);
      noteye_table_setInt(L, "type", ev.type == SDL_KEYDOWN ? evKeyDown : evKeyUp);
      noteye_table_setInt(L, "timestamp", kev.timestamp);
      noteye_table_setInt(L, "scancode", kev.keysym.scancode);
      noteye_table_setInt(L, "keycode", kev.keysym.sym);
      // had to change name because "repeat" is reserved in Lua
      if(kev.repeat)
        noteye_table_setInt(L, "repeated", kev.repeat);
      noteye_table_setInt(L, "mod", kev.keysym.mod);
      return true;
      }

    if(ev.type == SDL_TEXTINPUT) {
      lua_newtable(L);
      noteye_table_setInt(L, "type", evTextInput);
      noteye_table_setStr(L, "text", ev.text.text);
      return true;
      }

#ifdef SDL2    
    if(ev.type == SDL_WINDOWEVENT) {
      lua_newtable(L);
      noteye_table_setInt(L, "type", evWindowEvent);
      noteye_table_setInt(L, "timestamp", ev.window.timestamp);
      noteye_table_setInt(L, "subtype", ev.window.event);
      noteye_table_setInt(L, "data1", ev.window.data1);
      noteye_table_setInt(L, "data2", ev.window.data2);
      return true;
      }
#endif

    if(ev.type == SDL_MOUSEMOTION) {
      SDL_MouseMotionEvent& mev(ev.motion);
      lua_newtable(L);
      noteye_table_setInt(L, "type", evMouseMotion);
      noteye_table_setInt(L, "x", mev.x);
      noteye_table_setInt(L, "y", mev.y);
      noteye_table_setInt(L, "state", mev.state);
      return true;
      }

    if(ev.type == SDL_MOUSEWHEEL) {
      SDL_MouseWheelEvent& wev(ev.wheel);
      lua_newtable(L);
      noteye_table_setInt(L, "type", evMouseWheel);
      noteye_table_setInt(L, "dx", wev.x);
      noteye_table_setInt(L, "dy", wev.y);
#if (SDL_MAJOR_VERSION<<16 | SDL_MINOR_VERSION<<8 | SDL_PATCHLEVEL) >= 0x020004
      noteye_table_setInt(L, "direction", wev.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1);
#else
      noteye_table_setInt(L, "direction", 1); // older versions of SDL don't have this property
#endif
      return true;
      }

    if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP) {
      SDL_MouseButtonEvent& mev(ev.button);
      lua_newtable(L);
      noteye_table_setInt(L, "type", ev.type == SDL_MOUSEBUTTONDOWN ? evMouseDown : evMouseUp);
      noteye_table_setInt(L, "x", mev.x);
      noteye_table_setInt(L, "y", mev.y);
      noteye_table_setInt(L, "state", SDL_GetMouseState(NULL, NULL));
      noteye_table_setInt(L, "button", mev.button);
      noteye_table_setInt(L, "clicks", mev.clicks);
      noteye_table_setInt(L, "timestamp", mev.timestamp);
      return true;
      }

    if(ev.type == SDL_JOYAXISMOTION) {
      SDL_JoyAxisEvent& mev(ev.jaxis);
      lua_newtable(L);
      noteye_table_setInt(L, "type", evJoyAxis);
      noteye_table_setInt(L, "which", mev.which);
      noteye_table_setInt(L, "axis", mev.axis);
      noteye_table_setInt(L, "value", mev.value);
      return true;
      }

    if(ev.type == SDL_JOYBUTTONDOWN || ev.type == SDL_JOYBUTTONUP) {
      SDL_JoyButtonEvent& mev(ev.jbutton);
      lua_newtable(L);
      noteye_table_setInt(L, "type", ev.type == SDL_JOYBUTTONDOWN ? evJoyDown : evJoyUp);
      noteye_table_setInt(L, "which", mev.which);
      noteye_table_setInt(L, "button", mev.button);
      return true;
      }

    if(ev.type == SDL_JOYHATMOTION) {
      SDL_JoyHatEvent& mev(ev.jhat);
      lua_newtable(L);
      noteye_table_setInt(L, "type", evJoyHat);
      noteye_table_setInt(L, "which", mev.which);
      noteye_table_setInt(L, "hat", mev.hat);
      noteye_table_setInt(L, "value", mev.value);
      return true;
      }

    if(ev.type == SDL_JOYBALLMOTION) {
      SDL_JoyBallEvent& mev(ev.jball);
      lua_newtable(L);
      noteye_table_setInt(L, "type", evJoyBall);
      noteye_table_setInt(L, "which", mev.which);
      noteye_table_setInt(L, "ball", mev.ball);
      noteye_table_setInt(L, "xrel", mev.xrel);
      noteye_table_setInt(L, "yrel", mev.yrel);
      return true;
      }

    if(ev.type == SDL_QUIT) {
      lua_newtable(L);
      noteye_table_setInt(L, "type", evQuit);
      return true;
      }
    }
  return false;
  }
#endif

#ifdef USELUA
int lh_messagebox(lua_State *L) {
  checkArg(L, 4, "messagebox");
  return noteye_retInt(L,
    noteye_messagebox(luaInt(1), luaStr(2), luaStr(3), luaInt(4))
    );
  }
#endif

#ifdef WINDOWS

#include <SDL2/SDL_syswm.h>

void *getNoteyeWindowHWND(int window_id) {

  Window *win = dynamic_cast<Window*> (noteye_getobj(window_id));
  
  if(!win) return NULL;

  SDL_Window *sdl_window = win->win;

  SDL_SysWMinfo wminfo;
  SDL_VERSION(&wminfo.version);

  if (SDL_GetWindowWMInfo(sdl_window, &wminfo) != 1) {
    return NULL;
    }

  return wminfo.info.win.window;
  }

#endif

}

extern "C" {
int noteye_messagebox(Uint32 flags, const char* title, const char* message, int window_id) {
  noteye::Window *win = NULL;
  if (window_id > 0) {
    win = dynamic_cast<noteye::Window*> (noteye::noteye_getobj(window_id));
  }
  SDL_Window *sdl_window = win ? win->win : NULL;
  return SDL_ShowSimpleMessageBox(flags, title, message, sdl_window);
  }
}
