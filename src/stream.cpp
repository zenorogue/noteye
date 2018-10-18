// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

// streams to transfer

namespace noteye {

#define T (*this)

int writeUnc, writeCmp, readUnc, readCmp;

set<struct ImageAlias*> all_aliases;

struct ImageAlias : Object {
  smartptr<Image> img;
  string alias;
  ~ImageAlias();
  };

extern "C" {
ImageAlias *imagealias(Image *i, const char *alias) {
  ImageAlias *IA = new ImageAlias;
  IA->img = i;
  IA->alias = alias;
  all_aliases.insert(IA);
  return registerObject(IA);
  }

ImageAlias::~ImageAlias() {
  all_aliases.erase(this);
  }

}

void NStream::writeInt(int v) { 
  for(int i=0; i<4; i++) writeChar(v >> (i<<3));
  }
int NStream::readInt() {
  int v = 0;
  for(int i=0; i<3; i++) v += ((unsigned char)readChar()) << (i<<3);
  v += ((signed char)readChar()) << 24;
  return v;
  }
void NStream::writeDouble(double x) { 
  char* c = (char*) &x;
  for(int i=0; i<8; i++) writeChar(c[i]);
  }
double NStream::readDouble() {
  double x;
  char* c = (char*) &x;
  for(int i=0; i<8; i++) c[i] = readChar();
  return x;
  }

// === out stream ===

void NCompressedStream::writeChar(char c) {
  writeUnc++;
  if(!outok) {
    zout.next_in = outbuf_in;
    zout.avail_in = 0;
    outok = true;
    zout.zalloc = Z_NULL;
    zout.zfree = Z_NULL;
    zout.opaque = Z_NULL;
    deflateInit(&zout, Z_DEFAULT_COMPRESSION);
    }
  if(zout.avail_in == NOTEYESTREAMBUFSIZE) {
    while(zout.avail_in || !zout.avail_out) {
      zout.avail_out = NOTEYESTREAMBUFSIZE;
      zout.next_out = outbuf_out;
      deflate(&zout, 0);
      for(int i=0; i<int(NOTEYESTREAMBUFSIZE-zout.avail_out); i++) writeCharPrim(outbuf_out[i]), writeCmp++;
      }
    zout.next_in = outbuf_in;
    }
  outbuf_in[zout.avail_in++] = c;
  }

void NCompressedStream::flush() {
  while(zout.avail_in || !zout.avail_out) {
    zout.avail_out = NOTEYESTREAMBUFSIZE;
    zout.next_out = outbuf_out;
    deflate(&zout, Z_SYNC_FLUSH);
    for(int i=0; i<int(NOTEYESTREAMBUFSIZE-zout.avail_out); i++) writeCharPrim(outbuf_out[i]), writeCmp++;
    }
  zout.next_in = outbuf_in;
  }

// FILE * srttest = fopen("strtest", "wt");

char NCompressedStream::readChar() {
  proceed(false);
  if(finished) return 0;
  readUnc++;
  zin.avail_out++;
//  fputc(inbuf_out[cblock], srttest);
  return inbuf_out[cblock++];
  }

bool NCompressedStream::eof() {
//  fflush(srttest);
  proceed(true);
  return finished;
  }

bool NCompressedStream::ready() {
  proceed(true);
  return finished || zin.avail_out < NOTEYESTREAMBUFSIZE;
  }

void NCompressedStream::proceed(bool check) {
  if(finished) return;
  if(!inok) {
    zin.avail_in = 0;
    zin.avail_out = NOTEYESTREAMBUFSIZE;
    inok = true;
    zin.zalloc = Z_NULL;
    zin.zfree = Z_NULL;
    zin.opaque = Z_NULL;
    inflateInit(&zin);
    }
  while(zin.avail_out == NOTEYESTREAMBUFSIZE) {
    zin.next_out = inbuf_out; cblock = 0;
    if(!zin.avail_in) {
      if(check && !readyPrim()) {
        return;
        }
      if(eofPrim()) {
        finished = true;
        inflateEnd(&zin);
        return;
        }
      zin.next_in = inbuf_in;
      inbuf_in[0] = readCharPrim(); readCmp++;
      zin.avail_in++;
      }
    int ret = inflate(&zin, Z_SYNC_FLUSH);
    if(ret == Z_DATA_ERROR) {
      fprintf(errfile, "libz data error\n");      
      finished = true;
      }
    }
  }

void NStream::writeObj(Object *o) {
  if(o == NULL) {
    writeInt(0);
    return;
    }
  if(knownout.count(o)) {
    writeInt(knownout[o]);
    return;
    }
  int x = nextid++;
  knownout[o] = x;
  writeInt(x);
  if(true) {

    Get(Image, IM, o);
    if(IM) {
      writeInt(0x01);
      writeStr(IM->title);
      }

    Get(TileImage, TI, o);
    if(TI) {
      writeInt(0x11);
      writeInt(TI->ox);
      writeInt(TI->oy);
      writeInt(TI->sx);
      writeInt(TI->sy);
      writeInt(TI->trans);
      writeInt(TI->chid);
      writeObj(TI->i);
      }
    
    Get(TileMerge, TM, o);
    if(TM) {
      writeInt(TM->over ? 0x18 : 0x12);
      writeObj(TM->t1);
      writeObj(TM->t2);
      }
      
    // 0x13 was old recolor

    Get(TileSpatial, TSF, o);
    if(TSF) {
      writeInt(0x14);
      writeInt(TSF->sf);
      writeObj(TSF->t1);
      }
      
    Get(TileTransform, TT, o);
    if(TT) {
      writeInt(0x22);
      writeDouble(TT->dx);
      writeDouble(TT->dy);
      writeDouble(TT->sx);
      writeDouble(TT->sy);
      writeDouble(TT->dz);
      writeDouble(TT->rot);
      writeObj(TT->t1);
      }
      
    // 0x16-0x17 are reserved for old tilefills
    
    // note: 0x18 is reserved for TileMerge with over

    Get(TileLayer, TL, o);
    if(TL) {
      writeInt(0x19);
      writeObj(TL->t1);
      writeInt(TL->layerid);
      }
    
    Get(TileFill, TFI, o);
    if(TFI) {
      if(TFI->alpha == 0xffffff)
        writeInt(0x17);
      else if(TFI->alpha == 0x808080)
        writeInt(0x16);
      else
        writeInt(0x20), writeInt(TFI->alpha);
      writeInt(TFI->color);
      }              

    Get(TileRecolor, TR, o);
    if(TR) {
      writeInt(TR->mode == recDefault ? 0x13 : 0x21);
      writeInt(TR->color);
      writeObj(TR->t1);
      if(TR->mode != recDefault) writeInt(TR->mode);
      }
    }
  }

void NStream::writeScr(Screen *s) {
  writeInt(s->sx);
  writeInt(s->sy);
  for(int i=0; i<s->sy*s->sx; i++) writeObj(s->v[i]);
  }

void NStream::readScr(Screen *s) {
  int sx = readInt(), sy = readInt();
  s->setSize(sx, sy);
  for(int i=0; i<sy*sx; i++) s->v[i] = (Tile*) readObj();
  }

#ifdef USELUA

static lua_State *LS_image;

Image* imagenotfound(string s) {
  lua_State *L = LS_image;
  lua_pushvalue(L, -1);
  lua_pushstring(LS_image, s.c_str());
  if (lua_pcall(LS_image, 1, 1, 0) != 0) {
    noteyeError(34, "error running imagenotfound", lua_tostring(LS_image, -1));
    return 0;
    }
  int res = luaInt(-1);
  return dynamic_cast<Image*> (noteye_by_handle(res));
  }
  
#endif

Object *NStream::readObj() {
  int id = readInt();
  if(!knownin.count(id)) {
    int type = readInt();
    if(type == 0x1) {
      string s = readStr();
      // printf("Image titled %s\n", s.c_str()); fflush(stderr);
      Image *res = NULL;
      for(auto img: all_image_objects)
        if(img->title == s) { res = img; break; }
      if(!res) for(auto alias: all_aliases)
        if(alias->alias == s) { res = alias->img; break; }
      if(!res) {
        res = imagenotfound(s);
        }
      knownin[id] = res;
      }
    else if(type == 0x11) {
      TileImage *TI = new TileImage(0,0);
      TI->ox = readInt();
      TI->oy = readInt();
      TI->sx = readInt();
      TI->sy = readInt();
      TI->trans = readInt();
      TI->chid = readInt();
      TI->i = (Image*) readObj();
      knownin[id] = registerObject(TI);
      }
    else if(type == 0x12 || type == 0x18) {
      Tile *t1 = (Tile*) readObj();
      Tile *t2 = (Tile*) readObj();
      knownin[id] = addMerge(t1, t2, type == 0x18);
      }
    else if(type == 0x13) {
      int c = readInt(); 
      Tile *t1 = (Tile*) readObj();
      knownin[id] = addRecolor(t1, c, recDefault);
      }
    else if(type == 0x14) {
      TileSpatial TS;
      TS.sf = readInt();
      TS.t1 = (Tile*) readObj();
      knownin[id] = registerTile(TS);
      }
    else if(type == 0x19) {
      TileLayer TL;
      TL.t1 = (Tile*) readObj();
      TL.layerid = readInt();
      knownin[id] = registerTile(TL);
      }
    else if(type == 0x15) {
      TileTransform TT;
      TT.dx = readInt() / 1440.0;
      TT.dy = readInt() / 1440.0;
      TT.sx = readInt() / 1440.0;
      TT.sy = readInt() / 1440.0;
      TT.dz = 0;
      TT.rot= 0; 
      TT.t1 = (Tile*) readObj();
      knownin[id] = registerTile(TT);
      }
    else if(type == 0x16) {
      knownin[id] = addFill(readInt(), 0x808080);
      }
    else if(type == 0x17) {
      knownin[id] = addFill(readInt(), 0xffffff);
      }
    else if(type == 0x19) {
      Tile* t1 = (Tile*) readObj(); 
      int layerid = readInt();
      knownin[id] = addLayer(t1, layerid);
      }
    else if(type == 0x20) {
      int alpha = readInt(), color = readInt(); 
      knownin[id] = addFill(alpha, color);
      }
    else if(type == 0x21) {
      int c = readInt(); 
      Tile *t1 = (Tile*) readObj(); 
      int mode = readInt();
      knownin[id] = addRecolor(t1, c, mode);
      }
    else if(type == 0x22) {
      TileTransform TT;
      TT.dx = readDouble();
      TT.dy = readDouble();
      TT.sx = readDouble();
      TT.sy = readDouble();
      TT.dz = readDouble();
      TT.rot= readDouble();
      TT.t1 = (Tile*) readObj();
      knownin[id] = registerTile(TT);
      }
    else {
      noteyeError(31, "unknown type in stream", NULL, type);
      return 0;
      }
    }
  return knownin[id];
  }

NOFStream :: ~NOFStream() {
  if(outok) {
    flush(); 
    deflateEnd(&zout); 
    }
  if(f) fclose(f);
  }

extern "C" {

NOFStream* writefile(const char *s) {
  NOFStream *S = new NOFStream;
  S->f = fopen(s, "wb");
  if(!S->f) return NULL;
  return registerObject(S);
  }

NIFStream* readfile(const char *s) {
  NIFStream *S = new NIFStream;
  S->f = fopen(s, "rb");
  if(!S->f) return NULL;
  return registerObject(S);
  }

NStringStream* openstringstream() {
  NStringStream *S = new NStringStream;
  S->pos = 0;
  S->s = "";
  return registerObject(S);
  }

void resetknownout(NStream *S) {
  S->knownout.clear();
  }

void resetknownin(NStream *S) {
  S->knownin.clear();
  }

const char *getstringstream(NStringStream *S) {
  return S->s.c_str();
  }

void setstringstream(NStringStream *S, const char *str) {
  S->s = str; S->pos = 0;
  }

void writescr(NStream *S, Screen *SC) {
  S->writeScr(SC);
  }

void readscr(NStream *S, Screen *SC) {
  S->readScr(SC);
  }

void lh_writeint(NStream *S, int i) {
  S->writeInt(i);
  }

int readint(NStream *S) {
  return S->readInt();
  }

void writebyte(NStream *S, char x) {
  S->writeChar(x);
  }

char readbyte(NStream *S) {
  return S->readChar();
  }

void writestr(NStream *S, const char *str) {
  S->writeStr(str);
  }

const char *readstr(NStream *S) {
  static string last;
  last = S->readStr();
  return last.c_str();
  }

bool neof(NStream *S) {
  return S->eof();
  }

void nflush(NCompressedStream *S) {
  S->flush();
  }

void nfinish(NCompressedStream *S) {
  S->finish();
  }

bool nready(NCompressedStream *S) {
  return S->ready();
  }
}

// == network ==

#ifdef AVOID_NET

#define NETWORK 0

extern "C" {
TCPServer *nserver(int argument) { return NULL; }
TCPStream *naccept(TCPServer *SERV) { return NULL; }
TCPStream *nconnect(const char *addr, int port) { return NULL; }
}

#else

#define NETWORK 1
#ifndef LIBTCOD
#ifdef MAC
#include <SDL2_net/SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif
#endif

struct TCPServer : Object {
  TCPsocket socket;
  ~TCPServer();
  TCPServer(TCPsocket skt) : socket(skt) {}
  };

struct NTCPStream : NCompressedStream {
  TCPsocket socket;
  bool closed;
  FILE *f;
  void writeCharPrim(char c);
  char readCharPrim();
  bool readyPrim();
  ~NTCPStream();
  bool eofPrim() { return closed; }
  NTCPStream(TCPsocket skt) : socket(skt), closed(false) {}
  };

NStream *openTCPStream(void *skt) {
  return new NTCPStream(*(TCPsocket*)skt);
  }

void noteye_initnet() {
  static bool todo = true;
  if(todo) {
    todo = false;
    if(SDLNet_Init()==-1)
      noteyeError(32, "SDLNet_Init", SDLNet_GetError());
    }
  }

SDLNet_SocketSet sktset;

TCPsocket socketinset;

bool NTCPStream::readyPrim() {
  if(!sktset) {
    sktset = SDLNet_AllocSocketSet(1);
    // SDLNet_TCP_AddSocket(sktset,socket);
    }
  if(!sktset) 
    noteyeError(33, "allocsocket error", SDLNet_GetError());
  if(SDLNet_TCP_AddSocket(sktset,socket) < 0) return false;
  int ret = SDLNet_CheckSockets(sktset, 0);
  if(SDLNet_TCP_DelSocket(sktset,socket) < 0) return false;
  return ret;
  }

extern "C" {

TCPServer *nserver(int argument) {
  IPaddress ip;
  noteye_initnet();
  
  if(SDLNet_ResolveHost(&ip, NULL, argument) != 0) {
    fprintf(errfile, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());    
    return NULL;
    }
  TCPsocket skt = SDLNet_TCP_Open(&ip);
  if(!skt) {
    fprintf(errfile, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return NULL;
    }

  return registerObject(new TCPServer(skt));
  }

NTCPStream *naccept(TCPServer *SERV) {
  TCPsocket skt = SDLNet_TCP_Accept(SERV->socket);
  if(!skt) return NULL;
  return registerObject(new NTCPStream(skt));
  }

NTCPStream *nconnect(const char *addr, int port) {
  IPaddress ip;
  
  noteye_initnet();
  if(SDLNet_ResolveHost(&ip, addr, port) != 0) {
    fprintf(errfile, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());    
    return NULL;
    }
  TCPsocket skt = SDLNet_TCP_Open(&ip);
  if(!skt) {
    fprintf(errfile, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return NULL;
    }

  return registerObject(new NTCPStream(skt));
  }

}

#endif


#undef T

#ifndef AVOID_NET
TCPServer::~TCPServer() { if(socket) SDLNet_TCP_Close(socket); }

void NTCPStream::writeCharPrim(char   c) {
  if(SDLNet_TCP_Send(socket,&c,1) < 1) closed = true;
  }

char NTCPStream::readCharPrim() { 
  char c; 
  if(SDLNet_TCP_Recv(socket,&c,1) < 1) closed = true;
  return c;
  }

NTCPStream::~NTCPStream() { SDLNet_TCP_Close(socket); }
#endif


}
