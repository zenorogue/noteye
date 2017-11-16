// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#ifdef OPENGL
#if defined(MAC)
#include <OpenGL/glext.h>
#elif defined(ANDROID)
#include <GLES/glext.h>
#else
#include <GL/glext.h>
#endif

#ifdef ANDROID
#define glClearDepth glClearDepthf
#define glOrtho glOrthof
#define glFrustum glFrustumf
#define GL_BGRA GL_RGBA
#endif

namespace noteye {

// on some machines you need to use texture sizes which are powers of 2
#define POWEROFTWO

Window* useGL(Image *i) {
  Window *w = dynamic_cast<Window*> (i);
  if(!w) return NULL;
  if(w->flags & SDL_WINDOW_OPENGL) return w;
  return NULL;
  }

void glError(const char* GLcall, const char* file, const int line) {
  GLenum errCode = glGetError();
  if(errCode!=GL_NO_ERROR) {
    if(logfile) fprintf(logfile, "OPENGL ERROR #%i: in file %s on line %i :: %s\n",errCode,file, line, GLcall);
    fprintf(stderr, "OPENGL ERROR #%i: in file %s on line %i :: %s\n",errCode,file, line, GLcall);
    }
  }
#define GLERR(call) glError(call, __FILE__, __LINE__)

bool setContext(Window *w) {
  if(!w->gl) {
    w->gl = SDL_GL_CreateContext(w->win);
    if(!w->gl) {
      if(logfile)
        fprintf(logfile, "SDL error creating context: %s\n", SDL_GetError());
      fprintf(stderr, "SDL error creating context: %s\n", SDL_GetError());
      return false;
      }
    }
  if(SDL_GL_MakeCurrent(w->win, w->gl) < 0) {
    if(logfile)
      fprintf(logfile, "SDL error making current: %s\n", SDL_GetError());
    fprintf(stderr, "SDL error making current: %s\n", SDL_GetError());
    return false;
    }
  return true;
  }

void initOrthoGL(Window *w) {
  if(!setContext(w)) return;
  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0f); 
  glViewport(0, 0, w->sx, w->sy); 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); 
  glOrtho(0, w->sx, w->sy, 0, 1, -1); 
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  }

struct GLtexture {
  GLuint name;
  int cachechg;
  int maxxn, maxxd, maxyn, maxyd;
  };

void deleteTextureGL(TileImage *T) {
  if(T->gltexture) {
    glDeleteTextures(1, &T->gltexture->name);
    delete T->gltexture;
    T->gltexture = NULL;
    }
  }

void genTextureGL(TileImage *T) {
  GLERR("bitmap");
  if(!T->gltexture) {
    T->gltexture = new GLtexture;
    glGenTextures(1, &T->gltexture->name);
    T->gltexture->cachechg = -2;
    }
  if(T->gltexture->cachechg == T->i->changes) {
    glBindTexture(GL_TEXTURE_2D, T->gltexture->name);
    return;
    }
  int tsx = T->sx, tsy = T->sy;
#ifdef POWEROFTWO
  int i = 0;
  while(tsx > 1) i++, tsx >>= 1;
  tsx = 2; while(i) i--, tsx <<= 1;
  while(tsy > 1) i++, tsy >>= 1;
  tsy = 2; while(i) i--, tsy <<= 1;
  T->gltexture->maxxn = T->sx;
  T->gltexture->maxxd = tsx;
  T->gltexture->maxyn = T->sy;
  T->gltexture->maxyd = tsy;
#else
  T->gltexture->maxxn = 1;
  T->gltexture->maxyn = 1;
  T->gltexture->maxxd = 1;
  T->gltexture->maxyd = 1;
#endif
  T->gltexture->cachechg = T->i->changes;
  int *bitmap = new int[tsx * tsy];
  int *p = bitmap;
#ifdef POWEROFTWO
  for(int y=0; y<tsy; y++) for(int x=0; x<tsx; x++) *(p++) = 0;
  p = bitmap;
#endif
  SDL_Surface *src = T->i->s;
  for(int y=0; y<T->sy; y++) {
    for(int x=0; x<T->sx; x++) {    
      int px = qpixel(src, T->ox+x, T->oy+y);
      if(T->trans == transAlpha) *(p++) = px;
      else if(istrans(px, T->trans)) *(p++) = 0;
      else *(p++) = 0xFF000000 | px;
      }
    p += (tsx-T->sx);
    }
  glBindTexture(GL_TEXTURE_2D, T->gltexture->name);
  GLERR("bitmap");

#ifndef ANDROID
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tsx, tsy, 0, GL_BGRA, GL_UNSIGNED_BYTE, bitmap);
  GLERR("bitmap");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  GLERR("bitmap");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  delete[] bitmap;
  GLERR("bitmap");
  }

void disableGL(Window *w) {
  if(setContext(w)) {
    for(int i=0; i<(int) objs.size(); i++) {
      TileImage *TI = dbyId<TileImage> (i);
      if(TI) deleteTextureGL(TI);
      }
    }
  if(w->gl) SDL_GL_DeleteContext(w->gl);
  w->gl = NULL;
  }

struct coord_t {
  float x, y;
};

void drawTileImageGL(Window *dest, drawmatrix &M, TileImage *TI) {
  if(!setContext(dest)) return;
  genTextureGL(TI);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1,1,1,1);

#ifdef POWEROFTWO_NOFIX
  float dx = (1.0*TI->gltexture->maxxn) / TI->gltexture->maxxd;
  float dy = (1.0*TI->gltexture->maxyn) / TI->gltexture->maxyd;

  coord_t TexCoords[6] = {{0,0}, {dx,0}, {dx,dy},
                          {dx,dy}, {0, dy}, {0,0}};

  coord_t VertCoords[6] = {{M.x, M.y},
                           {M.x+M.tx, M.y+M.txy},
                           {M.x+M.tx+M.tyx, M.y+M.ty+M.txy},
                           {M.x+M.tx+M.tyx, M.y+M.ty+M.txy},
                           {M.x+M.tyx, M.y+M.ty},
                           {M.x, M.y}};
#else
  int txx = M.tx*TI->gltexture->maxxd/TI->gltexture->maxxn;
  int txy = M.txy*TI->gltexture->maxxd/TI->gltexture->maxxn;
  int tyy = M.ty*TI->gltexture->maxyd/TI->gltexture->maxyn;
  int tyx = M.tyx*TI->gltexture->maxyd/TI->gltexture->maxyn;

  coord_t TexCoords[6] = {{0,0}, {1,0}, {1,1},
                          {1,1}, {0, 1}, {0,0}};

  coord_t VertCoords[6] = {{M.x, M.y},
                           {M.x+txx, M.y+txy},
                           {M.x+txx+tyx, M.y+tyy+txy},
                           {M.x+txx+tyx, M.y+tyy+txy},
                           {M.x+tyx, M.y+tyy},
                           {M.x, M.y}};
  
  #endif

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords);
  glVertexPointer(2, GL_FLOAT, 0, &VertCoords);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glGetError();
  }

void drawFillGL(Window *dest, drawmatrix &M, TileFill *TF) {
  if(!setContext(dest)) return;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_TEXTURE_2D);
  noteyecolor col = TF->color;
  glColor4f(
    part(col,2)/255.0,
    part(col,1)/255.0,
    part(col,0)/255.0,
    (part(TF->alpha,0)+part(TF->alpha,1)+part(TF->alpha,2))/765.0
    );

  coord_t VertCoords[6] = {{M.x, M.y},
                           {M.x+M.tx, M.y+M.txy},
                           {M.x+M.tx+M.tyx, M.y+M.ty+M.txy},
                           {M.x+M.tx+M.tyx, M.y+M.ty+M.txy},
                           {M.x+M.tyx, M.y+M.ty},
                           {M.x, M.y}};

  glVertexPointer(2, GL_FLOAT, 0, &VertCoords);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glGetError();
  }

void fillRectGL(Window *dest, int x, int y, int w, int h, noteyecolor col) {
  if(!setContext(dest)) return;
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glColor4f(part(col,2)/255.0, part(col,1)/255.0, part(col,0)/255.0, 1.0f);

  coord_t VertCoords[6] = {{x,y},{x+w,y},{x+w,y+h},
                           {x+w,y+h},{x,y+h},{x,y}};

  glVertexPointer(2, GL_FLOAT, 0, &VertCoords);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  }

extern viewpar V;

static fpoint4 addShift(fpoint4 o, fpoint4 y, TileImage *w);

struct coord3d_t {
  float x,y,z;
};

void renderAffineImageGL(Window *dest, TileImage *w, fpoint4 orig, fpoint4 ox, fpoint4 oy) {
  if(!setContext(dest)) return;
  genTextureGL(w);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  coord3d_t VertCoords[6] = {
    {orig.x, orig.y, orig.z},
    {orig.x+ox.x, orig.y+ox.y, orig.z+ox.z},
    {orig.x+ox.x+oy.x, orig.y+ox.y+oy.y, orig.z+ox.z+oy.z},
    {orig.x+ox.x+oy.x, orig.y+ox.y+oy.y, orig.z+ox.z+oy.z},
    {orig.x+oy.x, orig.y+oy.y, orig.z+oy.z},
    {orig.x, orig.y, orig.z}};

  glVertexPointer(3, GL_FLOAT, 0, &VertCoords);

  if(w->sx == 1 && w->sy == 1) {
    glDisable(GL_TEXTURE_2D);
    // for some reason OpenGL does not work as expected with 1x1 textures
    noteyecolor col = qpixel(w->i->s, w->ox, w->oy);
    if(w->trans == transNone) col |= 0xFF000000;
    glColor4f(part(col,2)/255.0, part(col,1)/255.0, part(col,0)/255.0, part(col,3)/255.0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  else {
    if(V.shiftdown) orig = addShift(orig, oy, w);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1);
    float dx = (1.0*w->gltexture->maxxn) / w->gltexture->maxxd;
    float dy = (1.0*w->gltexture->maxyn) / w->gltexture->maxyd;

    coord_t TexCoords[6] = {{0,0}, {dx,0}, {dx,dy},
                            {dx,dy}, {0, dy}, {0,0}};

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords);
    }

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glGetError();
  }

void initFPPGL(Window *w) {
  if(!setContext(w)) return;
  glViewport(V.x0, w->sy-V.y1, V.x1-V.x0, V.y1-V.y0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double mz = 1;
  glFrustum(
    mz*(V.x0-V.xm)/V.xs, 
    mz*(V.x1-V.xm)/V.xs,
    mz*(V.ym-V.y1)/V.ys,
    mz*(V.ym-V.y0)/V.ys,
    mz, 1000);
  glScalef(V.camerazoom, V.camerazoom, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // float f = mz*V.ys/(V.y1-V.y0);
  glRotatef(V.cameraangle, 1, 0, 0);
  glRotatef(V.cameratilt, 0, 0, 1);
  glClearDepth( 1 );
  glClearColor(0,0,0,0);
  }

/*
void closeFPPGL() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, gfx.s->w, gfx.s->h, 0, 1, -1); 
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();
  } */

void refreshGL(Window *w) {
  if(!setContext(w)) return;
  SDL_GL_SwapWindow(w->win);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  }

void screenshotGL(Window *w, const char *fname) {
  if(!setContext(w)) return;
  SDL_Surface *s = 
    SDL_CreateRGBSurface(SDL_SWSURFACE, w->sx, w->sy, 32, 0xff0000, 0xff00, 0xff, 0xff000000);

  if(!s) return;

  glReadPixels(0, 0, w->sx, w->sy, GL_BGRA, GL_UNSIGNED_BYTE, s->pixels);
  
  for(int y=0; y<w->sy; y++) if(y*2 < w->sy)
  for(int x=0; x<w->sx; x++)
    swap(qpixel(s, x, y), qpixel(s, x, w->sy-1-y));

  SDL_SaveBMP(s, fname);
  SDL_FreeSurface(s);
  }

int getpixelGL(Window *w, int x, int y) {
  if(!setContext(w)) return 0;
  int p = 0;
  glReadPixels(x, w->sy-1-y, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, &p);
  return p;
  }

#endif
}
