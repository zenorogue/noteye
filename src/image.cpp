// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

long long totalimagesize = 0, totalimagecache = 0;

#define SURFACETYPE SDL_SWSURFACE
#define USEBLITS

set<Image*> all_image_objects;

Image::~Image() { 
  all_image_objects.erase(this);
  if(s) {
    IMGOBSERVE( printf("DEL %p image (%dx%d) title %sx\n", this, s->w, s->h, title.c_str()); )
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
 
int changecount = 0;

void Image::upchange() {
  changes = ++changecount;
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
  all_image_objects.insert(this);
  IMGOBSERVE( printf("NEW %p image (%dx%d) %s\n", this, s->w, s->h, title.c_str()); )

  convert();
  upchange();
  }

void Image::setLock(bool lock) {
  if(lock && !locked)
    SDL_LockSurface(s); 
  if(locked && !lock)
    SDL_UnlockSurface(s);
  locked = lock;
  }

Image::Image() : locked(false) { upchange(); }

Image::Image(int sx, int sy, noteyecolor color) : locked(false) {
  s = SDL_CreateRGBSurface(SURFACETYPE, sx, sy, 32, 0xFF<<16,0xFF<<8,0xFF,0xFF<<24);
  totalimagesize += sx * sy;
  IMGOBSERVE( printf("NEW %p image (%dx%d) color %08x\n", this, s->w, s->h, color); )
  SDL_LockSurface(s);
  SDL_UnlockSurface(s);
  SDL_FillRect(s, NULL, color);
  all_image_objects.insert(this);
  upchange();
  }

extern "C" {
Image *loadimage(const char *fname) {
  Image *o = new Image(fname);
  if(!o->s) { delete o; return NULL; }
  return registerObject(o);
  }

void saveimage(Image *o, const char *fname) {
  if(!o) {
    if(logfile) fprintf(logfile, "saveimage with invalid image\n");
    fprintf(stderr, "saveimage with invalid image\n");
    return;
    }
  if(useGL(o)) screenshotGL((Window*) o, fname);
  else if(useSDL(o)) screenshotSDL((Window*) o, fname);
  else SDL_SaveBMP(o->s, fname);
  }

Image *newimage(int x, int y) {
  return registerObject(new Image(x, y, 0));
  }

Image *newimage_color(int x, int y, int color) {
  return registerObject(new Image(x, y, color));
  }

void imagetitle(Image *o, const char *title) {
  if(o) o->title = title;
  else fprintf(stderr, "null imagetitle\n"); // todo
  }

void fillimage(Image *img, int x, int y, int w, int h, int col) {
  if(!img) { fprintf(stderr, "null imagetitle\n"); return; }
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
#ifdef OPENGL
  if(useGL(img)) fillRectGL(useGL(img), rect.x, rect.y, rect.w, rect.h, col); else
#endif
  if(useSDL(img)) fillRectSDL(useSDL(img), rect.x, rect.y, rect.w, rect.h, col); else
  SDL_FillRect(img->s, &rect, col);
  img->upchange();
  }

void imgcopy(Image *srcI, int srcX, int srcY, Image *tgtI, int tgtX, int tgtY, int six, int siy) {
  if(!srcI) { fprintf(stderr, "null imagetitle\n"); return; }
  if(!tgtI) { fprintf(stderr, "null imagetitle\n"); return; }
  srcI->setLock(false);
  tgtI->setLock(false);
  
  SDL_Rect srcR; srcR.x = srcX; srcR.y = srcY; srcR.w = six; srcR.h = siy;
  SDL_Rect tgtR; tgtR.x = tgtX; tgtR.y = tgtY; 
  
  SDL_BlitSurface(srcI->s, &srcR, tgtI->s, &tgtR);
  tgtI->upchange();
  }

noteyecolor img_getpixel(Image *srcI, int srcX, int srcY) {
  if(!srcI) { fprintf(stderr, "null imagetitle\n"); return 0; }
#ifdef OPENGL
  if(useGL(srcI)) return getpixelGL((Window*)srcI, srcX, srcY);
#endif
  if(useSDL(srcI)) return getpixelSDL((Window*)srcI, srcX, srcY);
  srcI->setLock(true);
  return qpixel(srcI->s, srcX, srcY);
  }

void img_setpixel(Image *srcI, int srcX, int srcY, noteyecolor pix) {
  if(!srcI) { fprintf(stderr, "null imagetitle\n"); return; }
#ifdef OPENGL
  if(useGL(srcI)) return;
#endif
  if(useSDL(srcI)) return; // todo

  srcI->setLock(true);
  qpixel(srcI->s, srcX, srcY) = pix;
  srcI->upchange();
  }

point imggetsize(Image *img) {
  if(!img) { point p; p.x = 0; p.y = 0; return p; }
  point p; p.x = img->s->w; p.y = img->s->h;
  return p;
  }
}

}
