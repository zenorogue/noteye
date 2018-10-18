// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#ifndef NOAUDIO
#ifdef MAC
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

extern "C" {
Sound *loadsound(const char *fname) {
  initAudio();
  if(audio == 1) return NULL;
  Mix_Chunk *chk = Mix_LoadWAV(fname);
  if(!chk) return NULL;
  Sound *snd = new Sound();
  snd->chunk = chk;
  return registerObject(snd);
  }

Music *loadmusic(const char *fname) {
  initAudio();
  if(audio == 1) return NULL;
  Mix_Music *chk = Mix_LoadMUS(fname);
  if(!chk) return NULL;
  Music *snd = new Music();
  snd->chunk = chk;
  return registerObject(snd);
  }

int playsound(Sound *s, int volume) {
  if(audio == 1 || !s) return 0;
  return s->play(volume);
  }

int playsoundloop(Sound *s, int volume, int loop) {
  if(audio == 1 || !s) return 0;
  return s->play(volume, loop);
  }

void musicvolume(int volume) {
  if(audio == 1) return;
  Mix_VolumeMusic(volume);
  }

void musichalt() {
  Mix_HaltMusic();
  mplaying = false;
  }

void playmusic(Music *m) {
  if(audio == 1 || !m) return;
  m->play();
  }

void playmusicloop(Music *m, int loop) {
  if(audio == 1 || !m) return;
  m->play(loop);
  }

void fadeoutmusic(int length) {
  if(audio == 1 || length == 0) return;
  Mix_FadeOutMusic(length);
  }

void pausemusic() {
  if(audio == 1) return;
  if(!mplaying) return;
  Mix_PauseMusic();
  }

void resumemusic() {
  if(audio == 1) return;
  if(!mplaying) return;
  Mix_ResumeMusic();
  }

bool musicon() { return mplaying; }

int mixsetdistance(int d1, int d2) {
  return Mix_SetDistance(d1, d2);
  }

int mixsetpanning(int p1, int p2, int p3) {
  return Mix_SetPanning(p1, p2, p3);
  }

int mixunregisteralleffects(int channel) {
  return Mix_UnregisterAllEffects(channel);
  }

}

#else
void closeAudio() {}
#endif

}
