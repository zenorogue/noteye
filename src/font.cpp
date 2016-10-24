// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

// -- bitmap fonts --

BitmapFont *newFont(Image *base, int inx, int iny, int trans) {
  BitmapFont *F = new BitmapFont;
  int sx = base->s ? base->s->w : 0;
  int sy = base->s ? base->s->h : 0;
  int dx = sx / inx;
  int dy = sy / iny;
  F->cnt = inx * iny;
  F->ti = new int[F->cnt];
  for(int i=0; i<inx*iny; i++) {
    F->ti[i] = 
      addTile(base, dx * (i % inx), dy * (i / inx), dx, dy, trans);
    if(F->ti[i]) 
      (byId<TileImage> (F->ti[i], NULL))->chid = i;
    }
  return F;
  }

int BitmapFont::gettile(int i) {
  if(i < 0 || i >= cnt) return 0;
  return ti[i];
  }

int BitmapFont::gettile(const char *s) {
  return gettile((unsigned char) s[0]);
  }

#ifdef USELUA
int lh_newfont(lua_State *L) {
  checkArg(L, 4, "newfont");
  return noteye_retObject(L, newFont(luaO(1, Image), luaInt(2), luaInt(3), luaInt(4)));
  }

int lh_getchar(lua_State *L) {
  checkArg(L, 2, "getchar");
  return noteye_retInt(L, luaO(1, Font)->gettile(luaStr(2)));
  }

int lh_getcharav(lua_State *L) {
  checkArg(L, 2, "getcharav");
  return noteye_retInt(L, luaO(1, Font)->gettile(luaInt(2)));
  }
#endif

// -- truetype fonts --

#ifndef LIBTCOD
TTFont *newTTFont(string fname) {
  if(TTF_Init() != 0) {
    if(errfile)
      fprintf(errfile, "Failed to initialize TTF.\n");
    }
  
  TTFont *F = new TTFont;
  F->fname = fname;
  return F;
  }
#endif

TTFont::~TTFont() {
  for(int i=0; i<size(sizes); i++) if(sizes[i]) TTF_CloseFont(sizes[i]);
  }

#ifdef USELUA
int lh_newttfont(lua_State *L) {
  checkArg(L, 1, "newttfont");
  return noteye_retObject(L, newTTFont(luaStr(1)));
  }
#endif

#ifndef LIBTCOD
TTF_Font* TTFont::getsize(int s) {
  if(s < 0 || s >= 1024) return NULL;
  while(size(sizes) <= s) sizes.push_back(NULL);
  if(sizes[s] == NULL) {
    sizes[s] = TTF_OpenFont(fname.c_str(), s);
    if(errfile && !sizes[s]) 
      fprintf(errfile, "could not load size %d of %s\n", s, fname.c_str());
    }
  return sizes[s];
  }
#endif

#ifdef USELUA
int lh_ttfgetsize(lua_State *L) {
  checkArg(L, 4, "ttfgetsize");
  TTFont* f = luaO(1, TTFont);
  int s = luaInt(2);
  string str = luaStr(3);
  int flags = luaInt(4);
  if(str == "") return 0;
  TTF_Font* tf = f->getsize(s);
  if(!tf) return 0;

  char buf[10000]; int bufpos = 0;
  if(str.size() > 9000) return 0;
  int omit = 0, omitnum = false, iix=0, iiy=0, totx=0, toty=0;
  int nid = 0;
  str = str + "\n";
  for(int i=0; i<int(str.size()); i++) {    
    if(omitnum) { 
      if(str[i] >= '0' && str[i] <= '9') 
        nid = 10*nid + str[i] - '0';
      else {
        Image *im = dbyId<Image> (nid);
        if(im) iix += im->s->w, iiy = max(iiy, im->s->h);
        }
      }
    else if(omit) {
      omit--; 
      if(!omit && str[i] == '#') omit = 6;
      if(!omit && str[i] == 'i') omitnum = true, nid=0; 
      }
    else if(str[i] == '\v') omit = 1;
    else if(str[i] == '\n') {
      if(bufpos) {
        buf[bufpos] = 0;
        int w, h;
        TTF_SizeUTF8(tf, buf, &w, &h);
        w += iix; h = max(h, iiy);
        totx = max(w, totx);
        toty += h;
        }
      }
    else buf[bufpos++] = str[i];
    }

  if(flags & 3) 
    totx += 4, toty += 4;
  
  lua_newtable(L);
  noteye_table_setInt(L, "x", totx);
  noteye_table_setInt(L, "y", toty);
  return 1; 
  }

enum AppendAlignment {
  aaHorizontal,
  aaVertical
  };
  
void appendImage(SDL_Surface*& target, SDL_Surface *s2, bool freeit, AppendAlignment aa) {
  SDL_Surface *t2 = convertSurface(s2);
  if(!target) target = t2;
  else if(aa == aaHorizontal) {
    SDL_Surface *t = target;
    int newh = max(t->h, t2->h);
    target = SDL_CreateRGBSurface(SURFACETYPE, t->w+t2->w, newh, 32, 0xFF<<16,0xFF<<8,0xFF,0xFF<<24);
    int u1 = (newh-t->h)/2;
    int u2 = (newh-t2->h)/2;
    for(int y=0; y<u1; y++) for(int x=0; x<t->w; x++)
      qpixel(target, x, y) = 0;
    for(int y=t->h+u1; y<newh; y++) for(int x=0; x<t->w; x++)
      qpixel(target, x, y) = 0;
    for(int y=0; y<u2; y++) for(int x=t->w; x<t->w+t2->w; x++)
      qpixel(target, x, y) = 0;
    for(int y=t2->h+u2; y<newh; y++) for(int x=t->w; x<t->w+t2->w; x++)
      qpixel(target, x, y) = 0;
    for(int y=0; y<t->h; y++) for(int x=0; x<t->w; x++)
      qpixel(target, x, y) = qpixel(t, x, y);
    for(int y=0; y<t->h; y++) for(int x=0; x<t2->w; x++)
      qpixel(target, x+t->w, y) = qpixel(t2, x, y);
    SDL_FreeSurface(t);
    SDL_FreeSurface(t2);
    }
  else if(aa == aaVertical) {
    SDL_Surface *t = target;
    int neww = max(t->w, t2->w);
    target = SDL_CreateRGBSurface(SURFACETYPE, neww, t->h+t2->h, 32, 0xFF<<16,0xFF<<8,0xFF,0xFF<<24);
    int u1 = (neww-t->w)/299;
    int u2 = (neww-t2->w)/299;
    for(int y=0; y<t->h; y++) {
      for(int x=0; x<u1; x++)
        qpixel(target, x, y) = 0;
      for(int x=0; x<t->w; x++)
        qpixel(target, x+u1, y) = qpixel(t, x, y);
      for(int x=t->w+u1; x<neww; x++)
        qpixel(target, x, y) = 0;
      }
    for(int y=0; y<t2->h; y++) {
      int ay = y+t->h;
      for(int x=0; x<u2; x++)
        qpixel(target, x, ay) = 0;
      for(int x=0; x<t2->w; x++)
        qpixel(target, x+u2, ay) = qpixel(t2, x, y);
      for(int x=t2->w+u2; x<neww; x++)
        qpixel(target, x, ay) = 0;
      }
    SDL_FreeSurface(t);
    SDL_FreeSurface(t2);
    }
    
  if(freeit) SDL_FreeSurface(s2);
  }

int lh_ttfrender(lua_State *L) {
  checkArg(L, 6, "ttfrender");
  TTFont* f = luaO(1, TTFont);
  int s = luaInt(2);
  string str = luaStr(3);
  noteyecolor color = luaInt(4);
  Image *i = luaO(5, Image);
  int flags = luaInt(6);
  
  int ncolor = color;
  
  TTF_Font *tf = f->getsize(s);
  if(!tf) return noteye_retInt(L, 0);
  
  SDL_Surface *cline = NULL, *plines = NULL;

  char buf[10000]; int bufpos = 0;
  if(str.size() > 9000) return 0;
  
  str = str + "\n0000000000"; // to protect against errors

  int colorstack[128], qcolorstack = 1;
  
  for(int ii=0; ii<int(str.size()); ii++) {
    if(str[ii] != '\v' && str[ii] != '\n') buf[bufpos++] = str[ii];
    else {
      if(bufpos) {
        SDL_Color col;
        col.r = (ncolor >> 16) & 255;
        col.g = (ncolor >> 8 ) & 255;
        col.b = (ncolor >> 0 ) & 255;
        buf[bufpos] = 0;
        appendImage(cline, TTF_RenderUTF8_Blended(tf, buf, col), true, aaHorizontal);
        bufpos = 0;
        }
      if(str[ii] == '\n') {
        appendImage(plines, cline, true, aaVertical);
        cline = NULL;
        continue;
        }
      ii++;
      if(qcolorstack < 1) qcolorstack = 1;
      if(qcolorstack > 127) qcolorstack = 127;
      char ch2 = str[ii];
      if(ch2 >= '0' && ch2 <= '9') {
        colorstack[qcolorstack++] = color;
        ncolor = vgacol[ch2 - '0'];
        continue;
        }
      if(ch2 >= 'a' && ch2 <= 'f') {
        colorstack[qcolorstack++] = color;
        ncolor = vgacol[ch2 - 'a' + 10];
        continue;
        }
      if(ch2 >= 'A' && ch2 <= 'F') {
        colorstack[qcolorstack++] = color;
        ncolor = vgacol[ch2 - 'A' + 10];
        continue;
        }
      if(ch2 == 'p') {
        ncolor = colorstack[--qcolorstack];
        continue;
        }
      if(ch2 == 'i') {
        // "\viXXX;" = insert image by number
        int id = 0;
        for(ii++; str[ii] >= '0' && str[ii] <= '9'; ii++) id = 10*id + str[ii] - '0';
        Image *im = dbyId<Image> (id);
        if(im) appendImage(cline, im->s, false, aaHorizontal);
        }
      if(ch2 == '#') {
        colorstack[qcolorstack++] = color;
        ncolor = 0;
        for(int i=0; i<6; i++) {
          ii++;
          char ch2 = str[ii];
          ncolor <<= 4;
          if(ch2 >= '0' && ch2 <= '9') ncolor += (ch2 - '0');
          if(ch2 >= 'a' && ch2 <= 'f') ncolor += (ch2 - 'a') + 10;
          if(ch2 >= 'A' && ch2 <= 'F') ncolor += (ch2 - 'A') + 10;
          }
        }
      }
    }
  
  if(i->s)
    SDL_FreeSurface(i->s);
  
  if(plines)
    i->s = plines;
  else {
    i->s = SDL_CreateRGBSurface(SURFACETYPE, 0, 0, 32, 0xFF<<16,0xFF<<8,0xFF,0xFF<<24);
    return noteye_retInt(L, 0);
    }

  if(flags & 3) {
    int w = 2;
    
    SDL_Surface* t = i->s;
    i->s = SDL_CreateRGBSurface(SURFACETYPE, t->w+w+w, t->h+w+w, 32, 0xFF<<16,0xFF<<8,0xFF,0xFF<<24);
    i->convert();
    for(int y=0; y<t->h+2; y++)
    for(int x=0; x<t->w+2; x++)
      qpixel(i->s, x, y) = 0;
    
    int wmap[5][5] = {
      {9,3,2,3,9},
      {3,2,1,2,3},
      {2,1,1,1,2},
      {3,2,1,2,3},
      {9,3,2,3,9}
      };

    for(int y=0; y<t->h; y++)
    for(int x=0; x<t->w; x++) {
      int p = qpixel(t, x, y);
      p &= 0xFF000000;
      if(p) for(int dy=2-w; dy<3+w; dy++) for(int dx=2-w; dx<3+w; dx++) if(wmap[dy][dx] >= (flags & 3))
        alphablend(qpixel(i->s, x+dx+w-2, y+dy+w-2), p);
      }

    for(int y=0; y<t->h; y++)
    for(int x=0; x<t->w; x++) {
      int p = qpixel(t, x, y);
      if(p & 0xFF000000) 
        alphablend(qpixel(i->s, x+w, y+w), p);
      }

    SDL_FreeSurface(t);
    }

  SDL_LockSurface(i->s);
  
  
  if(part(color, 3) < 255)
    for(int y=0; y<i->s->h; y++)
    for(int x=0; x<i->s->w; x++)
      part(qpixel(i->s, x, y), 3) = (part(qpixel(i->s, x, y), 3) * part(color, 3)) / 255;
  
  i->changes++;

  return noteye_retInt(L, 1);
  }
#endif

void DynamicFont::deleteLua() {
  if(ref == -1) return;
  luaL_unref(L, LUA_REGISTRYINDEX, ref);
  ref = -1;
  }

int DynamicFont::gettile(const char *s) {
  return gettile(utf8_decode(s));
  }

int DynamicFont::gettile(int i) {
  if(ti.count(i)) return ti[i];
  myuchar uc = utf8_encode_array(i);

  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  lua_pushstring(L, (char*) &uc);
  if (lua_pcall(L, 1, 1, 0) != 0) {
    noteyeError(16, "error running dynamicfont", lua_tostring(L, -1));
    return 0;
    }
  int res = noteye_argInt(L, -1);
  lua_pop(L, 1);
  
  ti[i] = res;
  return res;
  }

#ifdef USELUA
int lh_newdynamicfont(lua_State *L) {
  checkArg(L, 1, "newdynamicfont");
  lua_pushvalue(L, -1);
  DynamicFont *f = new DynamicFont;
  f->L = L;
  f->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  return noteye_retObject(L, f);
  }
#endif

}
