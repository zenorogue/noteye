// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

#define SERVERONLY
#define NOAUDIO
#define LIBTCOD
// This file is supposed to be included from libtcod.
// You can also base a NotEye for your own game on it

static struct lua_State *LS_image;

// SDL1 is included by libtcod, and we do not want NotEye to include SDL2
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_net.h>
// fake SDL2 structs
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_GLContext {};

#include <stdint.h>
typedef uint32_t Uint32;

#include "noteye.h"

#include "util.cpp"
#include "gfx.cpp"
#include "image.cpp"
#include "tiles.cpp"
#include "font.cpp"
#include "utf8.cpp"
#include "screen.cpp"

int imagenotfound(string s) {
  fprintf(stderr, "Image not found: %s\n", s.c_str());
  exit(2);
  }

#include "stream.cpp"

using namespace noteye;

static TCPServer *server;

#define MAXCLIENT 256

static NTCPStream *client[MAXCLIENT];

extern "C" {

static Image  *img;
//static Font   *fnt;
static Screen *scr;

TCOD_noteye tnoteye;

static int  lastframe = 0; // time of last frame rendered
static bool delayed;       // was the sending of the last screen delayed?

int *ourfont;
int fontcount;

void TCOD_noteye_init() {

  if(scr) return;
  
  if(tnoteye.port == 0) tnoteye.port = 6678;
  if(tnoteye.quality == 0) tnoteye.quality = 256;
  if(tnoteye.minTicks == 0) tnoteye.minTicks = 50;
  if(tnoteye.name == NULL) tnoteye.name = "libtcod";
  if(tnoteye.maxClients <= 0) tnoteye.maxClients = 1;
  if(tnoteye.maxClients > MAXCLIENT) tnoteye.maxClients = MAXCLIENT;
  
  objs.push_back(NULL);
  // objs.push_back(&gfx);  
  exsurface=SDL_CreateRGBSurface(SDL_SWSURFACE,16,16,32,0,0,0,0);

  img = new Image(8*32, 16*8);
  img->title = TCOD_ctx.font_file;
  registerObject(img);

  fontcount = TCOD_ctx.fontNbCharHoriz * TCOD_ctx.fontNbCharVertic;
  ourfont = new int[fontcount];

  for(int ch=0; ch<fontcount; ch++) {
    int ascii= TCOD_ctx.ascii_to_tcod[ch];
    
    int ti = 
      addTile(img, 
        (ascii%TCOD_ctx.fontNbCharHoriz)*TCOD_ctx.font_width, 
        (ascii/TCOD_ctx.fontNbCharHoriz)*TCOD_ctx.font_height, 
        TCOD_ctx.font_width, TCOD_ctx.font_height, 0);

    (byId<TileImage> (ti, LS_image))->chid = ch;

    ourfont[ch] = ti;
    }

  scr = new Screen;
  registerObject(scr);
  
  IPaddress ip;
  noteye_initnet();
  
  if(SDLNet_ResolveHost(&ip, NULL, tnoteye.port) != 0) {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    exit(1);
    }

  TCPsocket skt = SDLNet_TCP_Open(&ip);
  if(!skt) {
    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    exit(1);
    }
  
  server = new TCPServer(skt);
  }

int requal(int v) {
  int nq = tnoteye.quality;
  v *= nq;
  v /= 256;
  v *= 255;
  v /= (nq-1);
  return v;
  }

static int tcodtonoteyecolor(const TCOD_color_t& color) {
  int i = 0;
  i += requal(color.r) << 16;
  i += requal(color.g) << 8;
  i += requal(color.b) << 0;
  return i;
  }

char_t *tbuffer;

static void TCOD_noteye_render() {
  TCOD_noteye_init();
  int sx = TCOD_console_get_width(NULL);
  int sy = TCOD_console_get_height(NULL);
  scr->setSize(sx, sy);
  char_t *buffer = tbuffer;
  for(int y=0; y<sy; y++) for(int x=0; x<sx; x++) {
    scr->get(x,y) = 
      addMerge(
//      addFill(0), addRecolor(fnt->ti[buffer->c], 0xFFFFFF)
        addFill(tcodtonoteyecolor(buffer->back), 0xFFFFFF), addRecolor(ourfont[buffer->c], tcodtonoteyecolor(buffer->fore), recDefault),
        false
        );
    buffer++;
    }
  lastframe = SDL_GetTicks();
  }

void TCOD_noteye_sendscreen(char_t *buffer) {

  if(!tnoteye.mode) return;
  TCOD_noteye_init();

  tbuffer = buffer;
  delayed = int(SDL_GetTicks()) < lastframe + tnoteye.minTicks;
  if(delayed) return;
  TCOD_noteye_render();
  for(int i=0; i<tnoteye.maxClients; i++) if(client[i]) {
    // printf("Sending a screen... objs = %d hash = %d:%d\n", size(objs), hashok, hashcol);
    client[i]->writeInt(nepScreen);
    client[i]->writeScr(scr);
    client[i]->flush();
    }
  }

// translate the SDL2 scancode to SDL1 keysym
int scancodetokeysym(int sc) {
  switch(sc) {
    case 0: return 0;
    case 4: return 97;
    case 5: return 98;
    case 6: return 99;
    case 7: return 100;
    case 8: return 101;
    case 9: return 102;
    case 10: return 103;
    case 11: return 104;
    case 12: return 105;
    case 13: return 106;
    case 14: return 107;
    case 15: return 108;
    case 16: return 109;
    case 17: return 110;
    case 18: return 111;
    case 19: return 112;
    case 20: return 113;
    case 21: return 114;
    case 22: return 115;
    case 23: return 116;
    case 24: return 117;
    case 25: return 118;
    case 26: return 119;
    case 27: return 120;
    case 28: return 121;
    case 29: return 122;
    case 30: return 49;
    case 31: return 50;
    case 32: return 51;
    case 33: return 52;
    case 34: return 53;
    case 35: return 54;
    case 36: return 55;
    case 37: return 56;
    case 38: return 57;
    case 39: return 48;
    case 40: return 13;
    case 41: return 27;
    case 42: return 8;
    case 43: return 9;
    case 44: return 32;
    case 45: return 45;
    case 46: return 61;
    case 47: return 91;
    case 48: return 93;
    case 49: return 92;
    case 51: return 59;
    case 52: return 39;
    case 53: return 96;
    case 54: return 44;
    case 55: return 46;
    case 56: return 47;
    case 57: return 301;
    case 58: return 282;
    case 59: return 283;
    case 60: return 284;
    case 61: return 285;
    case 62: return 286;
    case 63: return 287;
    case 64: return 288;
    case 65: return 289;
    case 66: return 290;
    case 67: return 291;
    case 68: return 292;
    case 69: return 293;
    case 71: return 302;
    case 72: return 19;
    case 73: return 277;
    case 74: return 278;
    case 75: return 280;
    case 76: return 127;
    case 77: return 279;
    case 78: return 281;
    case 79: return 275;
    case 80: return 276;
    case 81: return 274;
    case 82: return 273;
    case 83: return 300;
    case 84: return 267;
    case 85: return 268;
    case 86: return 269;
    case 87: return 270;
    case 88: return 271;
    case 89: return 257;
    case 90: return 258;
    case 91: return 259;
    case 92: return 260;
    case 93: return 261;
    case 94: return 262;
    case 95: return 263;
    case 96: return 264;
    case 97: return 265;
    case 98: return 256;
    case 99: return 266;
    case 100: return 92;
    case 102: return 320;
    case 103: return 272;
    case 104: return 294;
    case 105: return 295;
    case 106: return 296;
    case 117: return 315;
    case 118: return 319;
    case 122: return 322;
    case 154: return 317;
    case 156: return 12;
    case 158: return 13;
    case 186: return 9;
    case 187: return 8;
    case 188: return 97;
    case 189: return 98;
    case 190: return 99;
    case 191: return 100;
    case 192: return 101;
    case 193: return 102;
    case 205: return 32;
    case 224: return 306;
    case 225: return 304;
    case 226: return 308;
    case 228: return 305;
    case 229: return 303;
    case 230: return 307;
    case 257: return 313;
    }
  return 0;
  }

SDL_Event bakev;

void TCOD_noteye_check() {
  if(!tnoteye.mode) return;
  TCOD_noteye_init();

  if(delayed) TCOD_noteye_sendscreen(tbuffer);
  
  int i;
  for(i=0; i < MAXCLIENT; i++) if(client[i] == NULL) break;
  if(i < MAXCLIENT && i < tnoteye.maxClients) {
    TCPsocket skt = SDLNet_TCP_Accept(server->socket);
    if(skt) {
      fprintf(stderr, "accepting a client\n");
      client[i] = new NTCPStream(skt);
      client[i]->writeStr("NotEye stream");
      client[i]->writeInt(NOTEYEVER);
      client[i]->writeStr(tnoteye.name);
      if(scr) {
        TCOD_noteye_render();
        client[i]->writeInt(nepScreen);
        client[i]->writeScr(scr);
        client[i]->flush();
        }
      }
    }
  
  for(int i=0; i < tnoteye.maxClients; i++) while(client[i] && client[i]->ready()) {
    if(client[i]->eof()) {
      delete client[i];
      client[i] = NULL;
      }
    else {
      int nep = client[i]->readInt();
      fprintf(stderr, "accepting a message #%d\n", nep);
      if(nep == nepKeyOld) {
        SDL_Event ev;
        SDL_KeyboardEvent& kev(ev.key);
        kev.keysym.sym = SDLKey(client[i]->readInt());
        kev.keysym.mod = SDLMod(client[i]->readInt());
        ev.type = client[i]->readInt() == evKeyDown ? SDL_KEYDOWN : SDL_KEYUP;
        kev.keysym.unicode = client[i]->readInt();
        SDL_PushEvent(&ev);
        }
      else if(nep == nepText) {
        // simulate SDL1 events
        SDL_Event ev = bakev;
        SDL_KeyboardEvent& kev(ev.key);
        string s = client[i]->readStr();
        kev.keysym.mod = SDLMod(0);
        for(int i=0; i<isize(s); i++) {
          kev.keysym.unicode = s[i]; ev.type = SDL_KEYDOWN; 
          // kev.keysym.sym = SDLKey(s[i]);
          SDL_PushEvent(&ev);
          kev.keysym.unicode = 0; ev.type = SDL_KEYUP;
          SDL_PushEvent(&ev);
          }
        }
      else if(nep == nepKeyNew) {
        // simulate SDL1 events
        SDL_Event ev;
        SDL_KeyboardEvent& kev(ev.key);
        
        int t = client[i]->readInt();
        int keycode = client[i]->readInt();
        int scancode = client[i]->readInt();
        int repeated = client[i]->readInt();
        if(repeated) ;
        int keymod = client[i]->readInt();
        
        fprintf(stderr, "t = %d\n", t);
        fprintf(stderr, "keycode = %d\n", keycode);

        switch(t) {
          case evKeyDown: case evKeyUp:
            ev.type = t == evKeyDown ? SDL_KEYDOWN : SDL_KEYUP;
            kev.keysym.mod = SDLMod(keymod);
            kev.keysym.sym = SDLKey(scancodetokeysym(scancode));
            if(kev.keysym.sym >= 32 && kev.keysym.sym < 128) {
              bakev = ev;
              break; // will be sent during nepText
              }            
            SDL_PushEvent(&ev);
            break;
          case evTextInput:
            kev.keysym.mod = SDLMod(0);
            kev.keysym.sym = SDLKey(0);
            kev.keysym.unicode = keycode;
            ev.type = SDL_KEYDOWN; SDL_PushEvent(&ev);
            ev.type = SDL_KEYUP; SDL_PushEvent(&ev);
            break;
          }
        }
      else if(nep == nepMouse) {
        tnoteye.x = client[i]->readInt();
        tnoteye.y = client[i]->readInt();
        tnoteye.state = client[i]->readInt();
        tnoteye.active = true;
        }
      else if(nep == nepMessage) {
        string s = client[i]->readStr();
        TCOD_noteye_writestr(s.c_str());
        }
      else fprintf(stderr, "Unknown NEP: %d\n", nep);
      }
    }
  }

void TCOD_noteye_writeint(int v) {
  for(int i=0; i<tnoteye.maxClients; i++) if(client[i]) 
    client[i]->writeInt(v);
  }

void TCOD_noteye_writestr(const char *v) {
  for(int i=0; i<tnoteye.maxClients; i++) if(client[i]) 
    client[i]->writeStr(v);
  }

void TCOD_noteye_flush() {
  for(int i=0; i<tnoteye.maxClients; i++) if(client[i]) 
    client[i]->flush();
  }

}

void noteye::noteye_halt() {}

TileImage::~TileImage() {}
