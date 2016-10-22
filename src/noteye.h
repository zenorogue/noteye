// Necklace of the Eye v8.3
// roguelike frontend
// Copyright (C) 2010-2016 Zeno Rogue, see 'noteye.h' for details

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// includes

#define SDL2

#ifndef _NOTEYE_H
#define _NOTEYE_H

#define NOTEYEVERSION "8.3"
#define NOTEYEVER 0x830
#define NOTEYEPATCH 6
#define NOTEYEPATCHSTR "P8"

#include <stdio.h>
#include <unistd.h>
#include <math.h>

#ifndef __cplusplus
typedef int bool;
#endif

#ifdef __cplusplus
#include <algorithm>
#include <complex>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#endif

// define SDLFLAT if SDL files do not use a subdirectory

#ifndef LIBTCOD
#ifdef SDLFLAT
#include <SDL.h>
#include <SDL_image.h>
#undef main
#elif defined(MAC)
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#ifndef NOTTF
#ifdef SDLFLAT
#include <SDL_ttf.h>
#elif defined(MAC)
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL2/SDL_ttf.h>
#endif
#endif

#ifdef OPENGL
#ifdef MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#ifndef NOAUDIO
#ifdef SDLFLAT
#include <SDL_mixer.h>
#elif defined(MAC)
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#endif
#endif

#include <zlib.h>

typedef struct lua_State lua_State;

// NotEye constants:
//===================

// use the alpha channel
#define transAlpha ((noteyecolor) (-0xABED))

// no transparent color (actually, we simply hope that this color does not appear)
#define transNone  ((noteyecolor) (-0xABEEAD))

// spatial flags

#define spFlat     1
#define spFloor    2
#define spCeil     4
#define spMonst    8
#define spItem     16
#define spCenter   32
#define spIFloor   64
#define spIItem    128

#define spICeil    0x00200
#define spIWallL   0x00400
#define spIWallR   0x00800
#define spWallN    0x01000
#define spWallE    0x02000
#define spWallS    0x04000
#define spWallW    0x08000
#define spFree     0x10000
#define spWallTop  0x20000
#define spWallBot  0x40000

// events

#define evKeyDown     1
#define evKeyUp       2
#define evProcScreen  3
#define evProcQuit    4
#define evMouseMotion 5
#define evMouseDown   6
#define evMouseUp     7
#define evBell        8
#define evQuit        9
// #define evResize      10 (OBSOLETE)
// #define evActive      11 (OBSOLETE)
#define evJoyAxis     12
#define evJoyUp       13
#define evJoyDown     14
#define evJoyHat      15
#define evJoyBall     16
#define evTextInput   17
#define evMouseWheel  18
#define evWindowEvent 19
#define evKeyConsole  20

// NotEye protocol

#define nepScreen     1
#define nepWait       2
#define nepKeyOld     3
#define nepFace       4
#define nepMode       5
#define nepMessage    6
#define nepCursor     7
#define nepMouse      8
#define nepText       9
#define nepKeyNew     10

// recoloring algorithms
#define recDefault    0
#define recMult       1
#define recPurple     2
#define recHue        3
#define recGamma      4

#define NOPARAM (-10000)

// how many keypresses to place in the buffer for the internal process
// why a large value: with a 80x25 roguelike, 4-way movement,
// moving a cursor to another corner could take (80+25)*2 ~= 200 events
#define EVENTBUFFER 640

#define NOTEYESTREAMBUFSIZE 4096


#ifdef __cplusplus 
namespace noteye {
#endif

// types
//=======

typedef unsigned char uint8;
typedef Uint32 noteyecolor;

// NotEye objects which can be accessed by the Lua script
//========================================================

#ifdef __cplusplus 
// Objects can be affected by Lua script

// basic object type
struct Object {
  int id;
  virtual bool checkEvent(struct lua_State *L) {return false;}
  virtual ~Object() {}
  };

// image
struct Image : Object {
  SDL_Surface *s;
  bool locked;
  int changes;
  void setLock(bool lock);
  Image(const char *fname);
  Image(int sx, int sy, noteyecolor color = 0);
  Image();
  ~Image();
  void convert();
  std::string title;
  };

// tile
struct Tile : Object {
  virtual void preprocess() {}
  Tile *nextinhash, **previnhash;
  Tile() : nextinhash(0), previnhash(0) {}
  ~Tile();
  virtual void debug() { printf("Tile\n"); }
  };

struct TileImage : Tile {
  Image *i;
  short ox, oy, sx, sy;
  noteyecolor trans;
  int chid; // character ID
  struct GLtexture *gltexture;
  struct SDLtexture *sdltexture;
  short tx, ty, bx, by; // bounding box
  int bcurrent; // is the bounding box correct? (bcurrent should equal i->changes)
  std::vector<struct TransCache*> caches;
  virtual int hash() const;
  virtual void debug();
  TileImage();
  TileImage(int _sx, int _sy);
  ~TileImage();
  };

struct TileMerge : Tile {
  int t1, t2;
  bool over;
  virtual int hash() const;
  virtual void debug();
  };

struct TileRecolor : Tile {
  int t1, mode;
  noteyecolor color;
  virtual int hash() const;
  int cache, cachechg;
  virtual void preprocess();
  virtual void recache(); // redraw the cache
  virtual void debug();
  ~TileRecolor();
  };

struct TileSpatial : Tile {
  int t1, sf;
  virtual int hash() const;
  virtual void debug();
  };

struct TileLayer : Tile {
  int t1, layerid;
  virtual int hash() const;
  virtual void debug();
  };

struct TileTransform : Tile {
  int t1;
  double dx, dy, sx, sy, dz, rot;
  virtual int hash() const;
  virtual void debug();
  };

struct FreeFormParam : Object {
  double d[4][4];
  // 0 = draw both sides
  // 1 = draw normal side only
  // 2 = draw back side only
  // 3 = reverse sides
  // 4 = do not change
  int side;
  // use 'shiftdown' on the resulting image
  bool shiftdown;
  };

// 3D point, used in the FPP mode
struct fpoint4 {
  double x, y, z;
  fpoint4() {}
  fpoint4(double X, double Y, double Z) {x=X; y=Y; z=Z;}
};

// viewpar struct for the FPP
struct viewpar {
  int x0, x1, y0, y1, xm, ym, xs, ys;
  double xz, yz;
  int ctrsize, monsize, objsize, monpush, objpush;
  bool shiftdown;
  int side;
  double cameraangle, cameratilt, camerazoom;
  fpoint4 delta;
  };

struct drawmatrix {
  int x, y, tx, ty, txy, tyx, tzx, tzy;
  };

struct TileFreeform : Tile {
  int t1;
  FreeFormParam *par;
  virtual int hash() const;
  virtual void debug();
  };

struct TileFill : Tile {
  noteyecolor color, alpha;
  virtual int hash() const;
  virtual void debug();
  TileImage *cache;
  };

// font
struct Font : Object {
  virtual int gettile(int id) = 0;
  virtual int gettile(const char* chr) = 0;
  };

struct BitmapFont : Font {
  int *ti;
  int cnt;
  ~BitmapFont() { delete[] ti; }
  int gettile(int id);
  int gettile(const char* chr);
  };

struct DynamicFont : Font { 
  int ref;
  struct lua_State *L;
  ~DynamicFont();
  std::map<int, int> ti;
  int gettile(int id);
  int gettile(const char* chr);
  };

#ifndef NOTTF
// TrueType font
struct TTFont : Object {
  std::string fname;
  std::vector<TTF_Font*> sizes;
  TTF_Font* getsize(int s);
  ~TTFont();
  };
#endif

// screen
struct Screen : Object {
  int sx, sy;
  std::vector<int> v;
  void setSize(int _sx, int _sy);
  void write(int x, int y, const char *buf, Font *f, int color);
  int& get(int x, int y);
  };

// tile mapping cache

extern std::set<struct TileMapping*> all_mappings;

struct TileMapping : Object {
  std::vector<int> cache;
  virtual int apply(int id);
  virtual void uncache(int id);
  virtual int applyRaw(int id) = 0;
  TileMapping() { all_mappings.insert(this); }
  ~TileMapping() { all_mappings.erase(this); }
  };

extern TileMapping 
  *tmFlat, *tmFloor, *tmCeil, *tmMonst, *tmItem, *tmCenter, *tmFloor, *tmItem,
  *tmICeil, *tmIWallL, *tmIWallR, *tmWallN, *tmWallE, *tmWallS, *tmWallW,
  *tmFree, *tmWallTop, *tmWallBot;

extern TileMapping *tmLayer[16];

// graphical window
struct Window : Image {
  int sx, sy, flags, renflags;
  std::string title;
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *tex;
  SDL_GLContext gl;
  bool usetex;
  Window() { win = NULL; ren = NULL; tex = NULL; }
  bool open(int x, int y, int flags, int renflags, int px, int py);
  void close();
  ~Window() { close(); }

  // create the surface (s) and texture (tex) of given sizse
  // (it need not be equal to sx,sy -- it might be an auxiliary surface for FPP)
  bool useSurfaceSize(int x, int y);
  };

// process
struct Process : Object {
  Screen *s;
  Font *f;
  const char *cmdline;
  bool isActive;
  int exitcode;
  bool active() { return isActive; }
  Process(Screen *scr, Font *f, const char *cmdline) : s(scr), f(f), cmdline(cmdline) {}
  int curx, cury;
  
  virtual int getCursorSize() = 0;

  virtual void sendKey(int scancode, int keycode, int mod, bool down) = 0;
  virtual void sendText(const std::string& s) = 0;
  };

// internal process
struct InternalProcess : Process {

  InternalProcess(Screen *scr, Font *f, const char *cmdline);
  ~InternalProcess();
  
  int back, fore, brushback, brush0;
  
  // int kbuf[IBUFSIZE], modbuf[IBUFSIZE], kbs, kbe, lastmod;
  
  union SDL_Event *evbuf[EVENTBUFFER], *lastevent;
  int evs, eve;

  void setColor(int fore, int back);
  int getCursorSize();    

  void lf();
  void drawChar(int c);
  int gp(int x, int dft);
  
  void applyM(int i, int& state);

  bool checkEvent(lua_State *L);
  void sendKey(int scancode, int keycode, int mod, bool down);
  void sendText(const std::string& s);
  
  bool changed;
  int  exitcode;
  };

// push tool
template <class T> struct push {
  T& x, saved;
  push(T& y, T val) : x(y) {saved=x; x=val;}
  ~push() {x = saved;}
  };

// stream
struct NStream : Object {
  std::set<int> knownout;       // for output streams
  std::map<int, int> knownin;   // for input  streams

  virtual void writeChar(char c) = 0;
  virtual char readChar() = 0;
  virtual bool eof() = 0;
  
  void writeInt(int v);
  void writeDouble(double x);
  int readInt();
  double readDouble();

  void writeStr(const std::string& s) {
    int sz = s.size();
    writeInt(sz);
    for(int i=0; i<sz; i++) writeChar(s[i]);
    }
  std::string readStr() {
    int size = readInt();
    std::string v;
    v.resize(size);
    for(int i=0; i<size; i++) v[i] = readChar();
    return v;
    }
  void writeObj(int x);
  int readObj();

  void readScr(Screen *s);
  void writeScr(Screen *s);  
  };

struct NCompressedStream : NStream {
  Bytef outbuf_in[NOTEYESTREAMBUFSIZE];
  Bytef outbuf_out[NOTEYESTREAMBUFSIZE];
  Bytef inbuf_in[NOTEYESTREAMBUFSIZE];
  Bytef inbuf_out[NOTEYESTREAMBUFSIZE];
  z_stream zout, zin;
  bool outok, inok, finished;
  int cblock;
  NCompressedStream() : outok(false), inok(false), finished(false) {}
  void finish() { finished = true; }

  // primitives
  virtual void writeCharPrim(char c) = 0;
  virtual char readCharPrim() = 0;
  virtual bool eofPrim() = 0;
  virtual bool readyPrim() = 0;

  void flush();
  void proceed(bool check);
  bool ready();
  void writeChar(char c);
  char readChar();
  bool eof();
  };

struct NOFStream : NCompressedStream {
  FILE *f;
  void writeCharPrim(char c) { fwrite(&c, 1, 1, f); }
  char readCharPrim() { printf("write-only\n"); exit(1); }
  ~NOFStream();
  bool eofPrim() { return feof(f); }
  bool readyPrim() { return true; }
  };

struct NIFStream : NCompressedStream {
  FILE *f;
  char readCharPrim() { char c; if(!fread(&c, 1, 1, f)) return -1; return c; }
  void writeCharPrim(char c) { printf("read-only\n"); exit(1); }
  ~NIFStream() { if(f) fclose(f); }
  bool eofPrim() { return feof(f); }
  bool readyPrim() { return true; }
  };

struct NStringStream : NStream {
  int pos;
  std::string s;
  void writeChar(char c) { 
    if(c == 0) { s += char(1); s += char(2); }
    else if (c == 1) { s += char(1); s += char(3); }
    else s += c; 
    }
  char readCharPrim() { 
    if(pos == (int) s.size()) return -1; 
    return s[pos++]; 
    }
  char readChar() {
    char c = readCharPrim();
    if(c == 1) return readCharPrim() - 2;
    return c;
    }
  bool eof() { return pos ==(int) s.size(); }
  };

// console output
struct MainScreen : public Screen {
  ~MainScreen();
  MainScreen();
  bool checkEvent(lua_State *L);
  };

// sound
#ifndef NOAUDIO
struct Sound : Object {
  Mix_Chunk *chunk;
  ~Sound();
  int play(int vol, int loops = 0);
  };

// music
struct Music : Object {
  Mix_Music *chunk;
  ~Music();
  void play(int loops);
  };
#endif

#else
struct Object;
struct Image;
struct Font;
struct TileImage;
#endif

// libNotEye functions
//=====================

#ifdef __cplusplus
extern "C" {
#endif

void noteye_args(int argc, char **argv);
void noteye_init();
void noteye_run(const char *noemain, bool applyenv);
void noteye_halt();

typedef void (*noteyehandler)(int id, const char *b1, const char *b2, int param);
void noteye_handleerror(noteyehandler h);

// Lua utils

void checkArg(struct lua_State *L, int q, const char *fname);

#define luaInt(x) noteye_argInt(L, x)
#define luaNum(x) noteye_argNum(L, x)
#define luaBool(x) noteye_argBool(L, x)
#define luaStr(x) noteye_argStr(L, x)

#define luaO(x,t) (byId<t> (luaInt(x), L))
#define dluaO(x,t) (dbyId<t> (luaInt(x)))

int noteye_argcount(struct lua_State *L);

int noteye_argInt(struct lua_State *L, int);
long double noteye_argNum(struct lua_State *L, int);
bool noteye_argBool(struct lua_State *L, int);
const char *noteye_argStr(struct lua_State *L, int);

int noteye_retInt(struct lua_State *L, int i);
int noteye_retBool(struct lua_State *L, bool b);
int noteye_retStr(struct lua_State *L, const char* s);

// you can use these to interact with Lua
int  noteye_table_new      (struct lua_State *L);
void noteye_table_opensub  (struct lua_State *L, const char *index);
void noteye_table_opensubAtInt  (struct lua_State *L, int index);
void noteye_table_closesub (struct lua_State *L);

void noteye_table_setInt   (struct lua_State *L, const char *index, int value);
void noteye_table_setNum   (struct lua_State *L, const char *index, double value);
void noteye_table_setStr   (struct lua_State *L, const char *index, const char *s);
void noteye_table_setBool  (struct lua_State *L, const char *index, bool b);
void noteye_table_setIntAtInt   (struct lua_State *L, int index, int value);
void noteye_table_setNumAtInt   (struct lua_State *L, int index, double value);
void noteye_table_setStrAtInt   (struct lua_State *L, int index, const char *s);
void noteye_table_setBoolAtInt  (struct lua_State *L, int index, bool b);

void noteye_globalint(const char *name, int value);
void noteye_globalstr(const char *name, const char *value);
void noteye_globalfun(const char *name, int f(lua_State *L));

struct Object *noteye_getobj(int id);
struct Object *noteye_getobjd(int id);
void noteye_wrongclass(int id, struct lua_State *L);

// make Curses calls refer to Prc, and call the object at stack position 'spos'
// on ghch

struct BitmapFont *newFont(struct Image *base, int inx, int iny, int trans);

int registerObject(struct Object* o);

struct InternalProcess *noteye_getinternal();

int noteye_getinternalx();
int noteye_getinternaly();

void noteye_setinternal(struct InternalProcess *Proc, lua_State *L, int spos);
void noteye_finishinternal(int exitcode);

#define CALLUI "noteye_callui"

void lua_stackdump(lua_State *L);
void noteye_uiresume();
void noteye_uifinish();

struct NStream *openTCPStream(void *skt);

void noteye_initnet();

int addTile(struct Image *i, int ox, int oy, int sx, int sy, int trans);
int addTileID(int id, int ox, int oy, int sx, int sy, int trans);
int addSpatial(int t1, int sf);
int addMerge(int t1, int t2, bool over);
int addMerge0(int t1, int t2);
int addMerge1(int t1, int t2);
int addRecolor(int t1, int color, int mode);
int addFill(int color, int alpha);
int addTransform(int t1, double dx, double dy, double sx, double sy, double dz, double rot);
int addLayer(int t1, int layerid);
int distillLayer(int x, int layerid);
int distill(int x, int sp);
int getChar(int x); 
int getBak(int x);
char getChar2(int i);
int getCol(int x);
int getImage(int x);

void saveimage(int id, const char *fname);
int scrget(int scr, int x, int y);
void scrset(int scr, int x, int y, int val);
void setdirectansi(int val);
void setconsolewindowtitle(const char *s);

int getFppDown(struct TileImage *T);

// functions useful for those who have no access to C++

#ifdef WINDOWS
void *getNoteyeWindowHWND(int window_id);
#endif

// SDL wrappers
int noteye_messagebox(Uint32 flags, const char* title, const char* message, int window_id);

#ifdef __cplusplus
  } // end extern C
#endif

// color mixing utilities:
#ifdef __cplusplus
uint8& part(noteyecolor& col, int i);
void applygamma(uint8& col, int gamma);
void recolor(noteyecolor& col, noteyecolor ncol, int mode);
void mixcolor(noteyecolor& col, noteyecolor ncol);
uint8 mixpart(uint8 a, uint8 b, uint8 c);
bool istrans(noteyecolor a, noteyecolor b);
bool istransA(noteyecolor a, noteyecolor b);
void mixcolorAt(noteyecolor& col, noteyecolor ncol, noteyecolor p);
void alphablend(noteyecolor& col, noteyecolor ncol);
void alphablendc(noteyecolor& col, noteyecolor ncol, bool cache);
#endif

#ifdef __cplusplus
template<class T> T* byId(int id, lua_State *L) {
  T* g = dynamic_cast<T*> (noteye_getobj(id));
  if(!g) noteye_wrongclass(id, L);
  return g;
  }

template<class T> T* dbyId(int id) {
  return dynamic_cast<T*> (noteye_getobjd(id));
  }
#endif

#ifdef __cplusplus
  } // end namespace
#endif

#endif
