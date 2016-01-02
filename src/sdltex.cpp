// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

// on some machines you need to use texture sizes which are powers of 2

Window* useSDL(Image *i) {
  Window *w = dynamic_cast<Window*> (i);
  if(!w) return NULL;
  if(w->flags & SDL_WINDOW_OPENGL) return NULL;
  if(!w->usetex) return NULL;
  return w;
  }

struct SDLtexture {
  SDL_Texture *t;
  int cachechg;
  };

void deleteTextureSDL(TileImage *T) {
  if(T->sdltexture) {
    SDL_DestroyTexture(T->sdltexture->t);
    delete T->sdltexture;
    T->sdltexture = NULL;
    }
  }

void genTextureSDL(Window *w, TileImage *T) {
  if(!T->sdltexture) {
    T->sdltexture = new SDLtexture;
    T->sdltexture->t = SDL_CreateTexture(
      w->ren, SDL_PIXELFORMAT_ARGB8888, 
      SDL_TEXTUREACCESS_STATIC, T->sx, T->sy
      );
    T->sdltexture->cachechg = -2;
    }
  if(T->sdltexture->cachechg == T->i->changes) return;

  T->sdltexture->cachechg = T->i->changes;
  int *bitmap = new int[T->sx * T->sy];
  int *p = bitmap;
  
  bool useblend = false;

  SDL_Surface *src = T->i->s;
  for(int y=0; y<T->sy; y++) {
    for(int x=0; x<T->sx; x++) {    
      noteyecolor& px(qpixel(src, T->ox+x, T->oy+y));
      if(T->trans == transAlpha) {
        *(p++) = px;
        if(part(px, 3) != 255) useblend = true;
        }
      else if(istrans(px, T->trans)) *(p++) = 0, useblend = true;
      else *(p++) = 0xFF000000 | px;
      }
    }
  
  SDL_SetTextureBlendMode(T->sdltexture->t, useblend ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

  SDL_UpdateTexture(T->sdltexture->t, NULL, bitmap, 4 * T->sx);

  delete[] bitmap;
  }

bool matrixIsStraight(const drawmatrix& M) {
  return M.txy == 0 && M.tyx == 0;
  }

const SDL_RendererFlip SDL_FLIP_BOTH = SDL_RendererFlip(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

void drawTileImageSDL(Window *dest, const drawmatrix &M, TileImage *TI) {

  genTextureSDL(dest, TI);
  
  SDL_Rect destrect;
  destrect.x = M.x;
  destrect.y = M.y;
  destrect.w = M.tx;
  destrect.h = M.ty;
  
  if(destrect.w < 0) { 
    destrect.w = -destrect.w; destrect.x -= destrect.w; 

    if(destrect.h < 0) {
      destrect.h = -destrect.h; destrect.y -= destrect.h; 
      SDL_RenderCopyEx(dest->ren, TI->sdltexture->t, NULL, &destrect, 0, NULL, SDL_FLIP_BOTH);
      }
    
    else      
      SDL_RenderCopyEx(dest->ren, TI->sdltexture->t, NULL, &destrect, 0, NULL, SDL_FLIP_HORIZONTAL);
    }

  else if(destrect.h < 0) { 
    destrect.h = -destrect.h; destrect.y -= destrect.h; 
    SDL_RenderCopyEx(dest->ren, TI->sdltexture->t, NULL, &destrect, 0, NULL, SDL_FLIP_VERTICAL);
    }
  
  else
    SDL_RenderCopy(dest->ren, TI->sdltexture->t, NULL, &destrect);
  }

void blitImageSDL(Window *dest, int x, int y, TileImage *TI) {

  genTextureSDL(dest, TI);
  
  SDL_Rect destrect;
  destrect.x = x;
  destrect.y = y;
  destrect.w = TI->sx;
  destrect.h = TI->sy;
  
  SDL_RenderCopy(dest->ren, TI->sdltexture->t, NULL, &destrect);
  }

void drawFillSDL(Window *dest, drawmatrix &M, TileFill *TF) {
  SDL_Rect destrect;
  destrect.x = M.x;
  destrect.y = M.y;
  destrect.w = M.tx;
  destrect.h = M.ty;
  
  if(destrect.w < 0) { 
    destrect.w = -destrect.w; destrect.x -= destrect.w; 
    }

  if(destrect.h < 0) { 
    destrect.h = -destrect.h; destrect.y -= destrect.y; 
    }

  int alpha = (part(TF->alpha,0)+part(TF->alpha,1)+part(TF->alpha,2) + 1) / 3;
  noteyecolor& col(TF->color);

  SDL_SetRenderDrawBlendMode(dest->ren, alpha == 255 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(dest->ren, part(col, 2), part(col, 1), part(col, 0), alpha);
  SDL_RenderFillRect(dest->ren, &destrect);
  }

void fillRectSDL(Window *dest, int x, int y, int w, int h, noteyecolor col) {

  SDL_Rect destrect;
  destrect.x = x;
  destrect.y = y;
  destrect.w = w;
  destrect.h = h;

  if(SDL_SetRenderDrawBlendMode(dest->ren, SDL_BLENDMODE_NONE))
    printf("blend error\n");
  if(SDL_SetRenderDrawColor(dest->ren, part(col, 2), part(col, 1), part(col, 0), part(col, 3)))
    printf("color error\n");
  if(SDL_RenderFillRect(dest->ren, &destrect))
    printf("fillrect error\n");
  }

extern viewpar V;

static fpoint4 addShift(fpoint4 o, fpoint4 y, TileImage *w);

void initFPPSDL(Window *w, int& x0, int& y0) {
  
  x0 = V.x0; y0 = V.y0;

  V.y1 -= y0; V.x1 -= x0; V.ym -= y0; V.xm -= x0; V.x0 = 0; V.y0 = 0;

  w->useSurfaceSize(V.x1, V.y1);
  SDL_FillRect(w->s, NULL, 0);
  }

void drawFPPSDL(Window *w, int x0, int y0) {
  
/*  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, gfx.s->w, gfx.s->h, 0, 1, -1); 
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); */
  
  SDL_Rect destrect;
  destrect.x = x0;
  destrect.y = y0;
  destrect.w = V.x1;
  destrect.h = V.y1;

  SDL_SetTextureBlendMode(w->tex, SDL_BLENDMODE_NONE);
  SDL_UpdateTexture(w->tex, NULL, w->s->pixels, w->s->pitch);
  SDL_RenderCopy(w->ren, w->tex, NULL, &destrect);
  
  V.x0 = x0; V.y0 = y0; V.y1 += y0; V.x1 += x0; V.ym += y0; V.xm += x0;
  }

void refreshSDL(Window *w) {
  SDL_RenderPresent(w->ren);
  }

void screenshotSDL(Window *w, const char *fname) {
  SDL_Surface *s = 
    SDL_CreateRGBSurface(SDL_SWSURFACE, w->sx, w->sy, 32, 0xff0000, 0xff00, 0xff, 0xff000000);

  if(!s) return;

  SDL_RenderReadPixels(w->ren, NULL, SDL_PIXELFORMAT_ARGB8888, s->pixels, s->pitch);
  
  SDL_SaveBMP(s, fname);
  SDL_FreeSurface(s);
  }

int getpixelSDL(Window *w, int x, int y) {
  int p = 0;
  SDL_Rect pix; pix.x = x; pix.y = y; pix.w = 1; pix.h = 1;
  SDL_RenderReadPixels(w->ren, &pix, SDL_PIXELFORMAT_ARGB8888, &p, 0);
  return p;
  }

void disableSDL(Window *w) {
  for(int i=0; i<(int) objs.size(); i++) {
    TileImage *TI = dbyId<TileImage> (i);
    if(TI) deleteTextureSDL(TI);
    }
  }

}
