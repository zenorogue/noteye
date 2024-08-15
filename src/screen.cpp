// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

#define check1(X) \
  if(X == 1) noteyeError(111, "suspicious image 1", lua_tostring(L, -1));

bool debugon;

lua_State *luamapstate;

tileptr outscr;

void Screen::setSize(int x, int y) {
  sx = x; sy = y;
  v.resize(sx * sy);
  for(int i=0; i<sx*sy; i++) v[i] = 0;
  }

extern "C" {
Screen* newScreen(int x, int y) {
  Screen *s = new Screen;
  s->setSize(x, y);
  return registerObject(s);
  }
}

void Screen::write(int x, int y, const char *buf, Font *f, noteyecolor color) {
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
    
    Tile *ti = f->gettile(ch);
    get(x,y) = addRecolor(ti, color, recDefault);
    x++;
    }
  }

tileptr& Screen::get(int x, int y) {
  if(x < 0 || x >=sx || y < 0 || y >= sy) return outscr;
  return v[y*sx+x];
  }

// --- lua

extern "C" {

void scrwrite(Screen *s, int x, int y, const char *buf, Font *f, noteyecolor color) {
  if(!s) { fprintf(stderr, "no screen\n"); return; }
  s->write(x, y, buf, f, color);
  }

void scrsetsize(Screen *s, int x, int y) {
  if(!s) { fprintf(stderr, "no screen\n"); return; }
  s->setSize(x, y);
  }

point scrgetsize(Screen *s) {
  point res;
  if(!s) { fprintf(stderr, "no screen\n"); return res; }
  res.x = s->sx;
  res.y = s->sy;
  return res;
  }

smartptr<Screen> prep_scrS, prep_tgtS;

void scrcopy(Screen *srcS, int srcX, int srcY, Screen *tgtS, int tgtX, int tgtY, int SX, int SY, TileMapping *utm) {
  if(!srcS) { fprintf(stderr, "no screen\n"); return; }
  if(!tgtS) { fprintf(stderr, "no screen\n"); return; }

  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) {
    tileptr& C1(srcS->get(srcX+x, srcY+y));
    tileptr& C2(tgtS->get(tgtX+x, tgtY+y));
    
    C2 = utm ? utm->apply(C1) : C1.base;
    }
  }

typedef Tile* (*tilecopyfunction)(Tile *t, int x, int y);

void scrcopy_f(Screen *srcS, int srcX, int srcY, Screen *tgtS, int tgtX, int tgtY, int SX, int SY, tilecopyfunction tcf) {
  if(!srcS) { fprintf(stderr, "no screen\n"); return; }
  if(!tgtS) { fprintf(stderr, "no screen\n"); return; }

  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) {
    tileptr& C1(srcS->get(srcX+x, srcY+y));
    tileptr& C2(tgtS->get(tgtX+x, tgtY+y));

    C2 = tcf(C1, srcX+x, srcY+y);
    }
  }

void scrfill(Screen *tgtS, int tgtX, int tgtY, int SX, int SY, Tile *t) {
  if(!tgtS) { fprintf(stderr, "no screen\n"); return; }
  
  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) {
    tgtS->get(tgtX+x, tgtY+y) = t;
    }
  }

void scrsave(Screen *srcS, const char *s, int mode) {
  if(!srcS) { fprintf(stderr, "no screen\n"); return; }

  FILE *f = fopen(s, "wt");
  if(!f) {
    fprintf(errfile, "could not save file '%s'\n", s);
    return;
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
  }

vector<int> layerstodraw;

int lh_uselayer(lua_State *L) {
  checkArg(L, 1, "uselayer");
  layerstodraw.push_back(luaInt(1));
  return 0;
  }

bool optimize_for_text;

void setOptimizeForText(bool b) { 
  optimize_for_text = b; 
  }

void drawScreen(Image *dest, Screen *s, int ox, int oy, int tx, int ty) {
  if(!dest) { fprintf(stderr, "no image\n"); return; }
  if(!s) { fprintf(stderr, "no screen\n"); return; }
  
  drawmatrix M;
  M.tx = tx; M.ty = ty;
  M.txy = M.tyx = M.tzx = M.tzy = 0;

  if(layerstodraw.size() == 0 && optimize_for_text) {  
    for(int ph=0; ph<2; ph++)
    for(int y=0; y<s->sy; y++)
      for(int x=0; x<s->sx; x++) {
        M.x = ox+x*tx; M.y = oy+y*ty;

        auto t = tmFlat->apply(s->get(x,y));

        auto TM = dynamic_cast<TileMerge*> (t);
        if(ph == 0) {
          if(TM) t = TM->t1;
          else continue;
          }
        else {
          if(TM) t = TM->t2;
          }

        drawTile(dest, M, t);
        }
    }
  else if(layerstodraw.size() == 0) {  
    for(int y=0; y<s->sy; y++)
      for(int x=0; x<s->sx; x++) {
        M.x = ox+x*tx; M.y = oy+y*ty;

        drawTile(dest, M, tmFlat->apply(s->get(x,y)));
        }
    }
  else {
    layerstodraw.push_back(-1);
    for(int pos = 0; pos < isize(layerstodraw); pos++) {
      for(int y=0; y<s->sy; y++) {
        for(int p=pos; layerstodraw[p] != -1; p++) {
          int lr = layerstodraw[p];
          for(int x=0; x<s->sx; x++) {
            M.x = ox+x*tx; M.y = oy+y*ty;
            drawTile(dest, M, tmFlat->apply(tmLayer[lr]->apply(s->get(x,y))));
            }
          }
        }
      while(layerstodraw[pos] != -1) pos++;
      }
    layerstodraw.clear();
    }
  
  dest->changes++;
  }

void drawScreenX(Image *dest, Screen *scr, int ox, int oy, int tx, int ty, TileMapping *utm) {

  if(!dest) { fprintf(stderr, "no image\n"); return; }
  if(!scr) { fprintf(stderr, "no screen\n"); return; }

  drawmatrix M;
  M.tx = tx; M.ty = ty;
  M.txy = M.tyx = M.tzx = M.tzy = 0;

  dest->changes++;
  
  if(optimize_for_text) {
    for(int ph=0; ph<2; ph++)
    for(int y=0; y<scr->sy; y++)
      for(int x=0; x<scr->sx; x++) {
        M.x = ox+x*tx; M.y = oy+y*ty;

        Tile *t = scr->get(x, y);     
        if(utm) t = utm->apply(t);
        t = tmFlat->apply(t);

        auto TM = dynamic_cast<TileMerge*> (t);
        if(ph == 0) {
          if(TM) t = TM->t1;
          else continue;
          }
        else {
          if(TM) t = TM->t2;
          }

        drawTile(dest, M, t);
        }
    return;
    }
    
  for(int y=0; y<scr->sy; y++)
    for(int x=0; x<scr->sx; x++) {

      Tile *C1 = scr->get(x, y);
     
      if(utm) C1 = utm->apply(C1);
  
      M.x = ox+x*tx; M.y = oy+y*ty;
      drawTile(dest, M, tmFlat->apply(C1));
      }
  }

void drawTile(Image *dest, Tile *t, int x, int y, int tx, int ty) {
  if(!dest) { fprintf(stderr, "no image\n"); return; }

  drawmatrix M;
  M.x = x; M.y = y;
  M.tx = tx; M.ty = ty;
  M.tyx = M.txy = M.tzx = M.tzy = 0;

  drawTile(dest, M, tmFlat->apply(t));
  dest->changes++;
  }
}

#define NOT_CACHED (-1)

set<struct TileMapping*> all_mappings;

tileptr cache_identity = registerObject(new Tile);

void TileMapping::uncache(Tile *t) {
  cache.erase(t);
  }

TileMapping::~TileMapping() {
  for(auto& p: cache) {
    if(!p.first) continue;
    auto& m = p.first->in_maps;
    for(int i=0; i<int(m.size()); i++) if(m[i] == this) { m[i] = m.back(); m.pop_back(); }
    }
  all_mappings.erase(this);
  }

Tile *TileMapping::apply(Tile *t) {
  if(cache.count(t)) {
    auto res = cache[t];
    if(res == cache_identity) return t;
    return res;
    }
  auto res = applyRaw(t);
  cache[t] = res == t ? cache_identity.base : res;
  if(t) t->in_maps.push_back(this);
  return res;
  }

struct TileMappingDistill : TileMapping {
  int flag;
  TileMappingDistill(int f) : flag(f) {}
  Tile* applyRaw(Tile *t) { return distill(t, flag); }
  };

struct TileMappingLayer : TileMapping {
  int layer;
  TileMappingLayer(int l) : layer(l) {}
  Tile* applyRaw(Tile *t) { return distillLayer(t, layer); }
  };

#ifdef USELUA

typedef Tile* (*tilemapper)(Tile *t);

extern std::set<struct TileMapping*> all_mappings;

struct TileMappingLua : TileMapping {
  tilemapper tm;
  TileMappingLua(tilemapper _tm) : tm(_tm) {}
  Tile* applyRaw(Tile *t) { return tm(t); }
  };
#endif

smartptr<TileMapping> tmFlat, tmFloor, tmCeil, tmMonst, tmItem, tmCenter, tmIFloor, tmIItem,
  tmICeil, tmIWallL, tmIWallR, tmWallN, tmWallE, tmWallS, tmWallW,
  tmFree, tmWallTop, tmWallBot, tmAllWall, tmCMI, tmLayer[16];

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

extern "C" {

TileMappingLua* newmapping(tilemapper tm) {
  return registerObject(new TileMappingLua(tm));
  }


Tile *mapapply(TileMapping *tmap, Tile *t) {
  if(!tmap) return NULL;
  return tmap->apply(t);
  }

Tile *scrget(Screen *s, int x, int y) {
  if(!s) return outscr;
  return s->get(x, y);
  }

void scrset(Screen *s, int x, int y, Tile *t) {
  if(!s) outscr = t;
  s->get(x,y) = t;
  }
}

}

