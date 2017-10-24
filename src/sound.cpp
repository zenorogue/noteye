// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#ifndef NOAUDIO
#ifdef SDLFLAT
#include <SDL_mixer.h>
#elif defined(MAC)
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif

namespace noteye {

int audio;
bool mplaying = false;

void musicFinished() { mplaying = false;}

void initAudio() {
  if(audio == 0) {  
//    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) != 0)  {
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0)  {
      fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
      audio = 1;
      }
    else audio = 2;
    Mix_AllocateChannels(16);
    Mix_HookMusicFinished(musicFinished);
    }
  }

extern "C" {
  void closeAudio() {
    if(audio == 2) Mix_CloseAudio();
    audio = 0;
    }
  }

int Sound::play(int vol, int loops) {
  Mix_VolumeChunk(chunk, vol);
  return Mix_PlayChannel(-1, chunk, loops);
  }

Sound::~Sound() {
  Mix_FreeChunk(chunk); 
  }

void Music::play(int loops = 0) {
  if(mplaying) Mix_HaltMusic();
  if(Mix_PlayMusic(chunk, loops) >= 0) mplaying = true;
  }

Music::~Music() {
  Mix_FreeMusic(chunk); 
  }

#ifdef USELUA
int lh_loadsound(lua_State *L) {
  checkArg(L, 1, "loadsound");
  initAudio();
  if(audio == 1) return noteye_retInt(L, 0);
  Mix_Chunk *chk = Mix_LoadWAV(luaStr(1));
  if(!chk) return noteye_retInt(L, 0);
  Sound *snd = new Sound();
  snd->chunk = chk;
  return noteye_retObject(L, snd);
  }

int lh_loadmusic(lua_State *L) {
  checkArg(L, 1, "loadmusic");
  initAudio();
  if(audio == 1) return noteye_retInt(L, 0);
  Mix_Music *chk = Mix_LoadMUS(luaStr(1));
  if(!chk) return noteye_retInt(L, 0);
  Music *snd = new Music();
  snd->chunk = chk;
  return noteye_retObject(L, snd);
  }

int lh_playsound(lua_State *L) {
  checkArg(L, 2, "playsound");
  if(audio == 1 || luaInt(1) == 0) return 0;
  Sound *s = luaO(1, Sound);
  return noteye_retInt(L, s->play(luaInt(2)));
  }

int lh_playsoundloop(lua_State *L) {
  checkArg(L, 3, "playsound");
  if(audio == 1 || luaInt(1) == 0) return 0;
  Sound *s = luaO(1, Sound);
  return noteye_retInt(L, s->play(luaInt(2), luaInt(3)));
  }

int lh_musicvolume(lua_State *L) {
  checkArg(L, 1, "musicvolume");
  if(audio == 1) return 0;
  int vol = luaInt(1);
  Mix_VolumeMusic(vol);
  return 0;
  }

int lh_musichalt(lua_State *L) {
  Mix_HaltMusic();
  mplaying = false;
  return 0;
  }

int lh_playmusic(lua_State *L) {
  checkArg(L, 1, "playmusic");
  if(audio == 1 || luaInt(1) == 0) return 0;
  Music *m = luaO(1, Music);
  m->play();
  return 0;
  }

int lh_playmusicloop(lua_State *L) {
  checkArg(L, 2, "playmusicloop");
  if(audio == 1 || luaInt(1) == 0) return 0;
  Music *m = luaO(1, Music);
  m->play(luaInt(2));
  return 0;
  }

int lh_fadeoutmusic(lua_State *L) {
  checkArg(L, 1, "playmusic");
  if(audio == 2) Mix_FadeOutMusic(luaInt(1));
  return 0;
  }

int lh_musicon(lua_State *L) {
  return noteye_retBool(L, mplaying);
  }

int lh_mixsetdistance(lua_State *L) {
  checkArg(L, 2, "mixsetdistance");
  return noteye_retInt(L, Mix_SetDistance(luaInt(1), luaInt(2)));
  }

int lh_mixsetpanning(lua_State *L) {
  checkArg(L, 3, "mixsetpanning");
  return noteye_retInt(L, Mix_SetPanning(luaInt(1), luaInt(2), luaInt(3)));
  }

int lh_mixunregisteralleffects(lua_State *L) {
  checkArg(L, 1, "mixunregisteralleffects");
  return noteye_retInt(L, Mix_UnregisterAllEffects(luaInt(1)));
  }

#endif

#else
void closeAudio() {}
#endif

}
