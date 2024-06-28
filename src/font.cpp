// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

// -- bitmap fonts --

extern "C" {
BitmapFont *newFont(Image *base, int inx, int iny, int trans) {
  BitmapFont *F = new BitmapFont;
  int sx = base->s ? base->s->w : 0;
  int sy = base->s ? base->s->h : 0;
  int dx = sx / inx;
  int dy = sy / iny;
  F->cnt = inx * iny;
  F->ti.resize(F->cnt);
  for(int i=0; i<F->cnt; i++) {
    F->ti[i] = 
      addTile(base, dx * (i % inx), dy * (i / inx), dx, dy, trans);
    if(F->ti[i]) {
      Get(TileImage, TI, F->ti[i]);
      if(TI) TI->chid = i;
      }
    }
  return registerObject(F);
  }
}

Tile *BitmapFont::gettile(int i) {
  if(i < 0 || i >= cnt) return NULL;
  return ti[i];
  }

Tile *BitmapFont::gettile(const char *s) {
  return gettile((unsigned char) s[0]);
  }

extern "C" {
Tile *fontgetchar(Font *F, const char *s) {
  if(!F) return NULL;
  return F->gettile(s);
  }

Tile *fontgetcharav(Font *F, int id) {
  if(!F) return NULL;
  return F->gettile(id);
  }
}

// -- truetype fonts --

#ifndef LIBTCOD
extern "C" {
TTFont *newTTFont(const char *fname) {
  if(TTF_Init() != 0) {
    if(errfile)
      fprintf(errfile, "Failed to initialize TTF.\n");
    }
  
  TTFont *F = new TTFont;
  F->fname = fname;
  return registerObject(F);
  }
  }
#endif

TTFont::~TTFont() {
  for(int i=0; i<isize(sizes); i++) if(sizes[i]) TTF_CloseFont(sizes[i]);
  }

#ifndef LIBTCOD
TTF_Font* TTFont::getsize(int s) {
  if(s < 0 || s >= 1024) return NULL;
  while(isize(sizes) <= s) sizes.push_back(NULL);
  if(sizes[s] == NULL) {
    sizes[s] = TTF_OpenFont(fname.c_str(), s);
    if(errfile && !sizes[s]) 
      fprintf(errfile, "could not load size %d of %s\n", s, fname.c_str());
    }
  return sizes[s];
  }
#endif

extern "C" {
point ttfgetsize(TTFont *f, int s, const char *bstr, int flags) {

  point res; res.x = 0; res.y = 0;
  
  if(bstr[0] == 0) return res;
  TTF_Font* tf = f->getsize(s);
  if(!tf) return res;

  char buf[10000]; int bufpos = 0;
  string str = bstr;
  if(str.size() > 9000) return res;
  int omit = 0, omitnum = false, iix=0, iiy=0, totx=0, toty=0;
  int nid = 0;
  str = str + "\n";
  for(int i=0; i<int(str.size()); i++) {    
    if(omitnum) { 
      if(str[i] >= '0' && str[i] <= '9') 
        nid = 10*nid + str[i] - '0';
      else {
        Image *im = dynamic_cast<Image*> (noteye_by_handle(nid));
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
  
  res.x = totx;
  res.y = toty;
  return res;
  }
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
    for(int y=0; y<t2->h; y++) for(int x=0; x<t2->w; x++)
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

extern "C" {

int ttfrender(TTFont *f, int s, const char *bstr, noteyecolor color, Image *i, int flags) {
  
  int ncolor = color;
  
  TTF_Font *tf = f->getsize(s);
  if(!tf) return 0;
  
  SDL_Surface *cline = NULL, *plines = NULL;

  string str = bstr;

  char buf[10000]; int bufpos = 0;
  if(str.size() > 9000) return 0;
  
  
  str = str + "\n0000000000"; // to protect against errors

  int colorstack[128], qcolorstack = 1;
  
  for(int ii=0; ii<isize(str); ii++) {
    if(str[ii] != '\v' && str[ii] != '\n') buf[bufpos++] = str[ii];
    else {
      if(bufpos) {
        SDL_Color col;
        col.a = (ncolor >> 24) & 255;
        col.r = (ncolor >> 16) & 255;
        col.g = (ncolor >> 8 ) & 255;
        col.b = (ncolor >> 0 ) & 255;
        buf[bufpos] = 0;
        appendImage(cline, TTF_RenderUTF8_Blended(tf, buf, col), true, aaHorizontal);
        bufpos = 0;
        }
      if(str[ii] == '\n') {
        if(cline) appendImage(plines, cline, true, aaVertical);
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
        Image *im = dynamic_cast<Image*> (noteye_by_handle(id));
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
    return 0;
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

  return 1;
  }
}

Tile *DynamicFont::gettile(const char *s) {
  return gettile(utf8_decode(s));
  }

Tile *DynamicFont::gettile(int i) {
  if(ti.count(i)) return ti[i];
  return ti[i] = f(i);
  }

extern "C" {
DynamicFont* newdynamicfont(dynamicfontfun f) {
  DynamicFont *F = new DynamicFont;
  F->f = f;
  return registerObject(F);
  }
}

}
