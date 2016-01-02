// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

// streams to transfer

namespace noteye {

#define T (*this)

int writeUnc, writeCmp, readUnc, readCmp;

struct ImageAlias : Object {
  Image *img;
  string alias;
  };

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

void NStream::writeObj(int x) {
  writeInt(x);
  if(x && !knownout.count(x)) {
    knownout.insert(x);

    Get(Image, IM, x);
    if(IM) {
      writeInt(0x01);
      writeStr(IM->title);
      }

    Get(TileImage, TI, x);
    if(TI) {
      writeInt(0x11);
      writeInt(TI->ox);
      writeInt(TI->oy);
      writeInt(TI->sx);
      writeInt(TI->sy);
      writeInt(TI->trans);
      writeInt(TI->chid);
      writeObj(TI->i->id);
      }
    
    Get(TileMerge, TM, x);
    if(TM) {
      writeInt(TM->over ? 0x18 : 0x12);
      writeObj(TM->t1);
      writeObj(TM->t2);
      }
      
    // 0x13 was old recolor

    Get(TileSpatial, TSF, x);
    if(TSF) {
      writeInt(0x14);
      writeInt(TSF->sf);
      writeObj(TSF->t1);
      }
      
    Get(TileTransform, TT, x);
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

    Get(TileLayer, TL, x);
    if(TL) {
      writeInt(0x19);
      writeInt(TL->t1);
      writeObj(TL->layerid);
      }
    
    Get(TileFill, TFI, x);
    if(TFI) {
      if(TFI->alpha == 0xffffff)
        writeInt(0x17);
      else if(TFI->alpha == 0x808080)
        writeInt(0x16);
      else
        writeInt(0x20), writeInt(TFI->alpha);
      writeInt(TFI->color);
      }              

    Get(TileRecolor, TR, x);
    if(TR) {
      writeInt(TR->mode == recDefault ? 0x13 : 0x21);
      writeInt(TR->color);
      writeObj(TR->t1);
      if(TR->mode != recDefault) writeObj(TR->mode);
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
  for(int i=0; i<sy*sx; i++) s->v[i] = readObj();
  }

#ifdef USELUA
static lua_State *LS_image;

int imagenotfound(string s) {
  lua_State *L = LS_image;
  lua_pushvalue(L, -1);
  lua_pushstring(LS_image, s.c_str());
  if (lua_pcall(LS_image, 1, 1, 0) != 0) {
    noteyeError(34, "error running imagenotfound", lua_tostring(LS_image, -1));
    return 0;
    }
  int res =luaInt(-1);
  lua_pop(LS_image, 1);
  byId<Image> (res, L);
  return res;
  }
  
#endif

int NStream::readObj() {
  int id = readInt();
  if(id == 0) return 0;
  if(!knownin.count(id)) {
    int type = readInt();
    int t1, ox, oy, c;
    if(type == 0x1) {
      string s = readStr();
      // printf("Image titled %s\n", s.c_str()); fflush(stderr);
      int res = 0;
      for(int i=1; i<size(objs); i++) {
        Image *img = dbyId<Image> (i);
        if(img && img->title == s) { res = i; break; }
        ImageAlias *alias = dbyId<ImageAlias> (i);
        if(alias && alias->alias == s) { res = alias->img->id; break; }
        }
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
      TI->i = byId<Image> (readObj(), LS_image);
      knownin[id] = registerObject(TI);
      }
    else if(type == 0x12 || type == 0x18) {
      ox = readObj(); oy = readObj();
      knownin[id] = addMerge(ox, oy, type == 0x18);
      }
    else if(type == 0x13) {
      c = readInt(); t1 = readObj();
      knownin[id] = addRecolor(t1, c, recDefault);
      }
    else if(type == 0x14) {
      TileSpatial TS;
      TS.sf = readInt();
      TS.t1 = readObj();
      knownin[id] = registerTile(TS);
      }
    else if(type == 0x19) {
      TileLayer TL;
      TL.t1 = readInt();
      TL.layerid = readObj();
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
      TT.t1 = readObj();
      knownin[id] = registerTile(TT);
      }
    else if(type == 0x16) {
      knownin[id] = addFill(readInt(), 0x808080);
      }
    else if(type == 0x17) {
      knownin[id] = addFill(readInt(), 0xffffff);
      }
    else if(type == 0x19) {
      t1 = readInt(); ox = readInt();
      knownin[id] = addLayer(t1, ox);
      }
    else if(type == 0x20) {
      ox = readInt(); t1 = readInt(); 
      knownin[id] = addFill(t1, ox);
      }
    else if(type == 0x21) {
      c = readInt(); t1 = readObj(); ox = readInt();
      knownin[id] = addRecolor(t1, c, ox);
      }
    else if(type == 0x22) {
      TileTransform TT;
      TT.dx = readDouble();
      TT.dy = readDouble();
      TT.sx = readDouble();
      TT.sy = readDouble();
      TT.dz = readDouble();
      TT.rot= readDouble();
      TT.t1 = readObj();
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

#ifdef USELUA
int lh_writefile(lua_State *L) {
  checkArg(L, 1, "writefile");
  NOFStream *S = new NOFStream;
  S->f = fopen(luaStr(1), "wb");
  if(!S->f) { delete S; return noteye_retInt(L, 0); }
  return noteye_retObject(L, S);
  }

int lh_readfile(lua_State *L) {
  checkArg(L, 1, "readfile");
  NIFStream *S = new NIFStream;
  S->f = fopen(luaStr(1), "rb");
  if(!S->f) { 
    delete S; 
    return noteye_retInt(L, 0); 
    }
  return noteye_retObject(L, S);
  }

int lh_openstringstream(lua_State *L) {
  checkArg(L, 1, "openstringstream");
  NStringStream *S = new NStringStream;
  S->pos = 0;
  S->s = "";
  return noteye_retObject(L, S);
  }

int lh_resetknownout(lua_State *L) {
  checkArg(L, 1, "resetknownout");
  NStream *S = luaO(1, NStream);
  S->knownout.clear();
  return 0;
  }

int lh_resetknownin(lua_State *L) {
  checkArg(L, 1, "resetknownin");
  NStream *S = luaO(1, NStream);
  S->knownin.clear();
  return 0;
  }

int lh_getstringstream(lua_State *L) {
  checkArg(L, 1, "getstringstream");
  NStringStream *S = luaO(1, NStringStream);
  return noteye_retStr(L, S->s);
  }

int lh_setstringstream(lua_State *L) {
  checkArg(L, 2, "setstringstream");
  NStringStream *S = luaO(1, NStringStream);
  S->s = luaStr(2); S->pos = 0;
  return 0;
  }

int lh_writescr(lua_State *L) {
  checkArg(L, 2, "writescr");
  NStream *S = luaO(1, NStream);
  Screen *SC = luaO(2, Screen);
  S->writeScr(SC);
  return 0;
  }

int lh_readscr(lua_State *L) {
  checkArg(L, 3, "readscr");
  NStream *S = luaO(1, NStream);
  Screen *SC = luaO(2, Screen);
  LS_image = L;
  S->readScr(SC);
  return 0;
  }

int lh_writeint(lua_State *L) {
  checkArg(L, 2, "writeint");
  luaO(1, NStream)->writeInt(luaInt(2));
  return 0;
  }

int lh_readint(lua_State *L) {
  checkArg(L, 1, "readint");
  lua_pushinteger(L, luaO(1, NStream)->readInt());
  return 1;
  }

int lh_writebyte(lua_State *L) {
  checkArg(L, 2, "writeint");
  luaO(1, NStream)->writeChar(luaInt(2));
  return 0;
  }

int lh_readbyte(lua_State *L) {
  checkArg(L, 1, "readint");
  lua_pushinteger(L, luaO(1, NStream)->readChar());
  return 1;
  }

int lh_writestr(lua_State *L) {
  checkArg(L, 2, "writestr");
  luaO(1, NStream)->writeStr(luaStr(2));
  return 0;
  }

int lh_readstr(lua_State *L) {
  checkArg(L, 1, "readstr");
  string s = luaO(1, NStream)->readStr();
  lua_pushstring(L, s.c_str());
  return 1;
  }

int lh_eof(lua_State *L) {
  checkArg(L, 1, "neof");
  lua_pushboolean(L, luaO(1, NStream)->eof());
  return 1;
  }

int lh_flush(lua_State *L) {
  checkArg(L, 1, "nflush");
  luaO(1, NCompressedStream)->flush();
  return 0;
  }

int lh_finish(lua_State *L) {
  checkArg(L, 1, "nfinish");
  luaO(1, NCompressedStream)->finish();
  return 0;
  }

int lh_ready(lua_State *L) {
  checkArg(L, 1, "nready");
  lua_pushboolean(L, luaO(1, NCompressedStream)->ready());
  return 1;
  }
#endif

// == network ==

#ifdef AVOID_NET

#define NETWORK 0

#ifdef USELUA
int lh_connect(lua_State *L) {
  checkArg(L, 2, "connect");
  return noteye_retInt(L, 0);
  }

int lh_accept(lua_State *L) {
  checkArg(L, 1, "accept");
  return noteye_retInt(L, 0);
  }

int lh_server(lua_State *L) {
  checkArg(L, 1, "server");
  return noteye_retInt(L, 0);
  }
#endif

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

#ifdef USELUA
int lh_server(lua_State *L) {
  checkArg(L, 1, "server");
  IPaddress ip;
  noteye_initnet();
  
  if(SDLNet_ResolveHost(&ip, NULL, luaInt(1)) != 0) {
    fprintf(errfile, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());    
    return noteye_retInt(L, 0);
    }
  TCPsocket skt = SDLNet_TCP_Open(&ip);
  if(!skt) {
    fprintf(errfile, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return noteye_retInt(L, 0);
    }

  return noteye_retObject(L, new TCPServer(skt));
  }

int lh_accept(lua_State *L) {
  checkArg(L, 1, "accept");
  TCPServer *SERV = luaO(1, TCPServer);
  TCPsocket skt = SDLNet_TCP_Accept(SERV->socket);
  if(!skt) return noteye_retInt(L, 0);
  return noteye_retObject(L, new NTCPStream(skt));
  }

int lh_connect(lua_State *L) {
  checkArg(L, 2, "connect");
  IPaddress ip;
  
  noteye_initnet();
  if(SDLNet_ResolveHost(&ip, luaStr(1), luaInt(2)) != 0) {
    fprintf(errfile, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());    
    return noteye_retInt(L, 0);
    }
  TCPsocket skt = SDLNet_TCP_Open(&ip);
  if(!skt) {
    fprintf(errfile, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return noteye_retInt(L, 0);
    }

  return noteye_retObject(L, new NTCPStream(skt));
  }

#endif

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

#ifdef USELUA
int lh_imagealias(lua_State *L) {
  checkArg(L, 2, "imagealias");
  ImageAlias *IA = new ImageAlias;
  IA->img = luaO(1, Image);
  IA->alias = luaStr(2);
  return noteye_retObject(L, IA);
  }

#endif

}
