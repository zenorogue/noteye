// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

#define check1(X) \
  if(X == 1) noteyeError(111, "suspicious image 1", lua_tostring(L, -1));

bool debugon;

// lua_State *luamapstate;

int outscr;

void Screen::setSize(int x, int y) {
  sx = x; sy = y;
  v.resize(sx * sy);
  for(int i=0; i<sx*sy; i++) v[i] = 0;
  }

Screen* newScreen(int x, int y) {
  Screen *s = new Screen;
  s->setSize(x, y);
  return s;
  }

void Screen::write(int x, int y, const char *buf, Font *f, int color) {
  int colorstack[128], qcolorstack = 1;
  while(*buf) {
    int len = utf8_numbytes(buf, 0);
    int ch = utf8_decode(buf, 0);
    buf += len;
    if(ch == '\v') {
      char ch2 = *(buf++);
      if(qcolorstack < 1) qcolorstack = 1;
      if(qcolorstack > 127) qcolorstack = 127;
      if(ch2 == 'v') ch = '\v';
      else if(ch2 == ':') ch = *(buf++);
      else if(ch2 >= '0' && ch2 <= '9') {
        colorstack[qcolorstack++] = color;
        color = vgacol[ch2 - '0'];
        continue;
        }
      else if(ch2 >= 'a' && ch2 <= 'f') {
        colorstack[qcolorstack++] = color;
        color = vgacol[ch2 - 'a' + 10];
        continue;
        }
      else if(ch2 >= 'A' && ch2 <= 'F') {
        colorstack[qcolorstack++] = color;
        color = vgacol[ch2 - 'A' + 10];
        continue;
        }
      else if(ch2 == 'p') {
        color = colorstack[--qcolorstack];
        continue;
        }
      else ch = '?';
      }
          
    int& c(get(x,y));
    int ti = f->gettile(ch);
    c = addRecolor(ti, color, recDefault);
    x++;
    }
  }

int& Screen::get(int x, int y) {
  if(x < 0 || x >=sx || y < 0 || y >= sy) return outscr;
  return v[y*sx+x];
  }

// --- lua

#ifdef USELUA
int lh_newScreen(lua_State *L) {
  checkArg(L, 2, "newscreen");
  Screen *s = newScreen(luaInt(1), luaInt(2));
  return noteye_retObject(L, s);
  }

int lh_scrwrite(lua_State *L) {
  checkArg(L, 6, "scrwrite");
  luaO(1, Screen)->write(luaInt(2), luaInt(3), luaStr(4), luaO(5, Font), luaInt(6));
  return 0;
  }

int lh_scrsetsize(lua_State *L) {
  checkArg(L, 3, "scrsetsize");
  luaO(1, Screen)->setSize(luaInt(2), luaInt(3));
  return 0;
  }

int lh_scrgetsize(lua_State *L) {
  checkArg(L, 1, "scrgetsize");
  Screen *scr = luaO(1, Screen);
  lua_newtable(L);
  noteye_table_setInt(L, "x", scr->sx);
  noteye_table_setInt(L, "y", scr->sy);
  return 1;
  }

int lh_scrcopy(lua_State *L) {
  checkArg(L, 9, "scrcopy");
  Screen *srcS = luaO(1, Screen);
  int srcX = luaInt(2);
  int srcY = luaInt(3);
  
  Screen *tgtS = luaO(4, Screen);
  int tgtX = luaInt(5);
  int tgtY = luaInt(6);
  
  int SX = luaInt(7);
  int SY = luaInt(8);
  
  int fid = lua_type(L, 9) == LUA_TFUNCTION ? -1 : luaInt(9);
  // luamapstate = L;
  TileMapping *utm = fid > 0 ? byId<TileMapping> (fid, L) : 0;

  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) {
    int& C1(srcS->get(srcX+x, srcY+y));
    int& C2(tgtS->get(tgtX+x, tgtY+y));
    
    if(fid == -1) {
      lua_pushvalue(L, -1);
      lua_pushinteger(L, C1);
      lua_pushinteger(L, srcX+x);
      lua_pushinteger(L, srcY+y);
      
      if (lua_pcall(L, 3, 1, 0) != 0) {
        noteyeError(15, "error running scrcopy", lua_tostring(L, -1));
        return 0;
        }
  
      C2 = luaInt(-1);
      lua_pop(L, 1);
      }
    else if(fid > 0) C2 = utm->apply(C1);
    else
      C2 = C1;
    }
  
  return 0;
  }

int lh_scrfill(lua_State *L) {
  checkArg(L, 6, "scrfill");
  
  Screen *tgtS = luaO(1, Screen);
  int tgtX = luaInt(2);
  int tgtY = luaInt(3);
  
  int SX = luaInt(4);  
  int SY = luaInt(5);
  
  int C = luaInt(6);
  
  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) {
    tgtS->get(tgtX+x, tgtY+y) = C;
    }
  
  return 0;
  }

int lh_scrsave(lua_State *L) {
  Screen *srcS = luaO(1, Screen);
  const char *s = luaStr(2);
  int mode = luaInt(3);
  FILE *f = fopen(s, "wt");
  if(!f) {
    fprintf(errfile, "could not save file '%s'\n", s);
    return 0;
    }

  int lcolor = getCol(srcS->get(0,0)) & 0xFFFFFF;

  fprintf(f, 
    mode ? "[tt][color=#%06x]" :
    "<html>\n"
    "<head>\n"
    "<meta name=\"generator\" content=\"Necklace of the Eye\">\n"
    "<title>HTML Screenshot</title>\n"
    "</head>\n"
    "<body bgcolor=#0><pre><font color=#%06x>\n", lcolor
    );
  
  for(int y=0; y<srcS->sy; y++) {
    for(int x=0; x<srcS->sx; x++) {
      int ncolor = getCol(srcS->get(x,y)) & 0xFFFFFF;
      char c = getChar(srcS->get(x,y));
      if(c == 0) c = 32;
      if(ncolor != lcolor && c != 32) {
        fprintf(f, mode ? "[/color][color=#%06x]" : "</font><font color=#%06x>", ncolor);
        lcolor = ncolor;
        }
      fprintf(f, "%c", c);
      }
    fprintf(f, "\n");
    }
  fprintf(f, mode ? "[/color][/tt]" : "</font></body></html>\n");

  fclose(f);
  return 0;
  }

int lh_drawScreen(lua_State *L) {
  check1(luaInt(1));
  Image *dest = luaO(1, Image);
  Screen *scr = luaO(2, Screen);
  int ox = luaInt(3); int oy = luaInt(4);
  int tx = luaInt(5); int ty = luaInt(6);
  
  drawmatrix M;
  M.tx = tx; M.ty = ty;
  M.txy = M.tyx = M.tzx = M.tzy = 0;
  
  for(int y=0; y<scr->sy; y++)
    for(int x=0; x<scr->sx; x++) {
      M.x = ox+x*tx; M.y = oy+y*ty;
      drawTile(dest, M, tmFlat->apply(scr->get(x,y)));
      }
  
  dest->changes++;
  return 0;
  }

int lh_drawScreenX(lua_State *L) {
  check1(luaInt(1));
  Image *dest = luaO(1, Image);
  Screen *scr = luaO(2, Screen);
  int ox = luaInt(3); int oy = luaInt(4);
  int tx = luaInt(5); int ty = luaInt(6);
  
  int fid = lua_type (L, 7) == LUA_TFUNCTION ? -1 : luaInt(7);
  // luamapstate = L;
  TileMapping *utm = fid > 0 ? byId<TileMapping> (fid, L) : 0;
  
  drawmatrix M;
  M.tx = tx; M.ty = ty;
  M.txy = M.tyx = M.tzx = M.tzy = 0;

  dest->changes++;
  for(int y=0; y<scr->sy; y++)
    for(int x=0; x<scr->sx; x++) {

      int C1 = scr->get(x, y);
      
      if(fid == -1) {
        lua_pushvalue(L, -1);
        lua_pushinteger(L, C1);
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        
        if (lua_pcall(L, 3, 1, 0) != 0) {
          noteyeError(16, "error running drawScreenX", lua_tostring(L, -1));
          return 0;
          }
        C1 = luaInt(-1);
        lua_pop(L, 1);
        }
      else if(fid > 0)
        C1 = utm->apply(C1);
  
      M.x = ox+x*tx; M.y = oy+y*ty;
      drawTile(dest, M, tmFlat->apply(C1));
      }
  
  return 0;
  }

int lh_drawTile(lua_State *L) {
  check1(luaInt(1));
  Image *dest = luaO(1, Image);
  drawmatrix M;
  M.x = luaInt(3); M.y = luaInt(4);
  M.tx = luaInt(5); M.ty = luaInt(6);
  M.tyx = M.txy = M.tzx = M.tzy = 0;

  if(luaInt(2) < 0) printf("apply flat to -1\n");
  
  drawTile(dest, M, tmFlat->apply(luaInt(2)));
  dest->changes++;
  return 0;
  }
#endif

#define NOT_CACHED (-1)

set<struct TileMapping*> all_mappings;

void TileMapping::uncache(int id) {
  if(int(cache.size()) > id)
    cache[id] = NOT_CACHED;
  }

int TileMapping::apply(int id) {
  if(id < 0 || id >= int(objs.size())) {
    noteyeError(36, "odd object ID in tileMapping", NULL, id);
    return 0;
    }
  while(int(cache.size()) < id+1)
    cache.push_back(NOT_CACHED);
  if(cache[id] != NOT_CACHED) return cache[id];
  return cache[id] = applyRaw(id);
  }

struct TileMappingDistill : TileMapping {
  int flag;
  TileMappingDistill(int f) : flag(f) {}
  int applyRaw(int id) { return distill(id, flag); }
  };

struct TileMappingLayer : TileMapping {
  int layer;
  TileMappingLayer(int l) : layer(l) {}
  int applyRaw(int id) { return distillLayer(id, layer); }
  };

#ifdef USELUA
struct TileMappingLua : TileMapping {
  lua_State *L;
  int ref;
  TileMappingLua(lua_State *L2, int r) : L(L2), ref(r) {}
  void deleteLua() { 
    if(ref == -1) return;
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    ref = -1;
    }

  ~TileMappingLua() { deleteLua(); }
  int applyRaw(int id) { 
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_pushinteger(L, id);
    if (lua_pcall(L, 1, 1, 0) != 0) {
      noteyeError(16, "error running TileMapping", lua_tostring(L, -1));
      return 0;
      }
    int ret = noteye_argInt(L, -1);
    lua_pop(L, 1);
    return ret;
    }
  };
#endif

TileMapping *tmFlat, *tmFloor, *tmCeil, *tmMonst, *tmItem, *tmCenter, *tmIFloor, *tmIItem,
  *tmICeil, *tmIWallL, *tmIWallR, *tmWallN, *tmWallE, *tmWallS, *tmWallW,
  *tmFree, *tmWallTop, *tmWallBot, *tmAllWall, *tmCMI;

TileMapping *tmLayer[16];

#define Q(x) \
  tm##x = new TileMappingDistill(sp##x); registerObject(tm##x);

void initMappings() {
  Q(Flat) Q(Floor) Q(Ceil) Q(Monst) Q(Item) Q(Center) Q(IFloor) Q(IItem)
  Q(ICeil) Q(IWallL) Q(IWallR) Q(WallN) Q(WallE) Q(WallS) Q(WallW) Q(Free)
  Q(WallTop) Q(WallBot)
  tmAllWall = new TileMappingDistill(spWallN | spWallS | spWallE | spWallW | spWallTop | spWallBot);
  tmCMI = new TileMappingDistill(spCenter | spMonst | spItem);
  
  for(int i=0; i<16; i++) {
    tmLayer[i] = new TileMappingLayer(i); registerObject(tmLayer[i]);
    }
  }

#undef Q

#ifdef USELUA
int lh_newmapping(lua_State *L) {
  checkArg(L, 1, "newmapping");
  lua_pushvalue(L, -1);
  return noteye_retObject(L, new TileMappingLua(L, luaL_ref(L, LUA_REGISTRYINDEX)));
  }

int lh_mapapply(lua_State *L) {
  checkArg(L, 2, "mapapply");
  int id = luaInt(2);
  if(luaInt(1) == 0) return noteye_retInt(L, id);
  TileMapping *tmap = luaO(1, TileMapping);
  return noteye_retInt(L, tmap->apply(id));
  }
#endif

int scrget(int scr, int x, int y) {
  Screen* s = dbyId<Screen> (scr);
  return s ? s->get(x,y) : 0;
  }

void scrset(int scr, int x, int y, int val) {
  Screen* s = dbyId<Screen> (scr);
  if(s) s->get(x,y) = val;
  }

}

