// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

long long totalimagesize = 0, totalimagecache = 0;

#define SURFACETYPE SDL_SWSURFACE
#define USEBLITS

Image::~Image() { 
  if(s) {
    totalimagesize -= s->w * s->h;
    SDL_FreeSurface(s);
    }
  }

SDL_Surface *convertSurface(SDL_Surface* s) {
  SDL_PixelFormat fmt;
  fmt.format = SDL_PIXELFORMAT_BGRA8888;
  fmt.BitsPerPixel = 32;
  fmt.BytesPerPixel = 4;

  fmt.Ashift=24;
  fmt.Rshift=16; 
  fmt.Gshift=8; 
  fmt.Bshift=0; 
  fmt.Amask=0xff<<24;
  fmt.Rmask=0xff<<16;  
  fmt.Gmask=0xff<<8;  
  fmt.Bmask=0xff;  
  fmt.Aloss = fmt.Rloss = fmt.Gloss = fmt.Bloss = 0;
  fmt.palette = NULL;

#ifndef SDL2
  fmt.alpha = 0;
  fmt.colorkey = 0x1ffffff;
#endif
  
  return SDL_ConvertSurface(s, &fmt, SURFACETYPE);
  }

void Image::convert() {
  SDL_Surface *s2;
  // count the number of changes, so we update caches when required
  
  if(0) {
//  SDL_PixelFormat fmt = s2->format;
//  s2->format->Amask = 0xff << 24;
//  s2->format->Ashift = 24;
    s2 = SDL_ConvertSurface(s, exsurface->format, SURFACETYPE);
    }
  else s2 = convertSurface(s);

  // SDL_DisplayFormat(s);
  if(!s2) {
    fprintf(stderr, "error = %s\n", SDL_GetError());
    noteyeError(0, "could not change image to display format", NULL);
    }

  SDL_FreeSurface(s);
  s = s2;
  }
 
Image::Image(const char *fname) : locked(false) {
  title = fname;
  #ifndef LIBTCOD
  s = IMG_Load(fname);
  #endif
  if(!s) {
    fprintf(errfile, "Image file missing: %s\n", fname);
    return;
    }
  totalimagesize += s->w * s->h;

  convert();
  changes = 0;
  }

void Image::setLock(bool lock) {
  if(lock && !locked) SDL_LockSurface(s); if(locked && !lock) SDL_UnlockSurface(s); locked = lock;
  }

Image::Image() : locked(false) { changes = 0; }

Image::Image(int sx, int sy, noteyecolor color) : locked(false) {
  s = SDL_CreateRGBSurface(SURFACETYPE, sx, sy, 32, 0xFF<<16,0xFF<<8,0xFF,0xFF<<24);
  totalimagesize += sx * sy;
  SDL_LockSurface(s);
  SDL_UnlockSurface(s);
  SDL_FillRect(s, NULL, color);
  changes = 0;
  }

#ifdef USELUA
int lh_loadimage(lua_State *L) {
  checkArg(L, 1, "loadimage");
  Image *o = new Image(luaStr(1));
  if(!o->s) { delete o; return noteye_retInt(L, 0); }
  return noteye_retObject(L, o);
  }

void saveimage(int id, const char *fname) {
  Image *o = dbyId<Image> (id);
  if(!o) {
    if(logfile) fprintf(logfile, "saveimage with invalid image\n");
    fprintf(stderr, "saveimage with invalid image\n");
    return;
    }
  if(useGL(o)) screenshotGL((Window*) o, fname);
  else if(useSDL(o)) screenshotSDL((Window*) o, fname);
  else SDL_SaveBMP(o->s, fname);
  }

int lh_newimage(lua_State *L) {
  int params = lua_gettop(L);
  if(params != 2 && params != 3) {
    noteyeError(1, "Bad arg to newimage", NULL); 
    return noteye_retInt(L, 0);
    }
  Image *i = new Image(luaInt(1), luaInt(2), params == 3 ? luaInt(3) : 0);
  return noteye_retObject(L, i);
  }

int lh_imagetitle(lua_State *L) {
  checkArg(L, 2, "imagetitle");
  luaO(1, Image) -> title = luaStr(2);
  return 0;
  }

int lh_fillimage(lua_State *L) {
  checkArg(L, 6, "fillimage");
  SDL_Rect rect;
  rect.x = luaInt(2);
  rect.y = luaInt(3);
  rect.w = luaInt(4);
  rect.h = luaInt(5);
  int col = luaInt(6);
  Image *img = luaO(1,Image);
#ifdef OPENGL
  if(useGL(img)) fillRectGL(useGL(img), rect.x, rect.y, rect.w, rect.h, col); else
#endif
  if(useSDL(img)) fillRectSDL(useSDL(img), rect.x, rect.y, rect.w, rect.h, col); else
  SDL_FillRect(img->s, &rect, col);
  img->changes++;
  return 0;
  }

int lh_imgcopy(lua_State *L) {
  checkArg(L, 8, "fillimage");

  Image *srcI = luaO(1, Image);
  int srcX = luaInt(2);
  int srcY = luaInt(3);

  Image *tgtI = luaO(4, Image);
  int tgtX = luaInt(5);
  int tgtY = luaInt(6);
  
  int six = luaInt(7);
  int siy = luaInt(8);
  
  srcI->setLock(false);
  tgtI->setLock(false);
  
  SDL_Rect srcR; srcR.x = srcX; srcR.y = srcY; srcR.w = six; srcR.h = siy;
  SDL_Rect tgtR; tgtR.x = tgtX; tgtR.y = tgtY; 
  
  SDL_BlitSurface(srcI->s, &srcR, tgtI->s, &tgtR);
  tgtI->changes++;
  return 0;
  }

int img_getpixel2(Image *srcI, int srcX, int srcY) {
#ifdef OPENGL
  if(useGL(srcI)) return getpixelGL((Window*)srcI, srcX, srcY);
#endif
  if(useSDL(srcI)) return getpixelSDL((Window*)srcI, srcX, srcY);
  srcI->setLock(true);
  return qpixel(srcI->s, srcX, srcY);
  }

noteyecolor img_getpixel(int src, int srcX, int srcY) {
  Image *srcI = dbyId<Image> (src);
  if(!srcI) {
    if(logfile) fprintf(logfile, "getpixel with invalid image\n");
    fprintf(stderr, "getpixel with invalid image\n");
    return;
    }
  return img_getpixel2(srcI, srcX, srcY);
  }

int img_setpixel2(Image *srcI, int srcX, int srcY, int pix) {
#ifdef OPENGL
  if(useGL(srcI)) return 0;
#endif
  if(useSDL(srcI)) return 0; // todo

  srcI->setLock(true);
  qpixel(srcI->s, srcX, srcY) = pix;
  srcI->changes++;
  return 0;
  }

void img_setpixel(int src, int srcX, int srcY, noteyecolor pix) {
  Image *srcI = dbyId<Image> (src);
  if(!srcI) {
    if(logfile) fprintf(logfile, "setpixel with invalid image\n");
    fprintf(stderr, "setpixel with invalid image\n");
    return;
    }
  img_setpixel2(srcI, srcX, srcY, pix);
  }

int lh_imggetsize(lua_State *L) {
  checkArg(L, 1, "imggetsize");
  Image *img = luaO(1, Image);
  lua_newtable(L);
  noteye_table_setInt(L, "x", img->s->w);
  noteye_table_setInt(L, "y", img->s->h);
  return 1;
  }

#endif

}
