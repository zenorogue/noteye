// Necklace of the Eye v8.3
// roguelike frontend
// Copyright (C) 2010-2017 Zeno Rogue, see 'noteye.h' for details

// On-screen keyboard and textinput functions

#ifdef SDL2
namespace noteye {

#ifdef USELUA
extern "C" {
bool SDL_HasScreenKeyboardSupport() {
  return ::SDL_HasScreenKeyboardSupport() == SDL_TRUE;
  }

bool SDL_IsScreenKeyboardShown(Window *w) {
  return ::SDL_IsScreenKeyboardShown(w->win) == SDL_TRUE;
  }
}

int lh_SDL_IsTextInputActive(lua_State *L) {
  return noteye_retBool(L, SDL_IsTextInputActive() == SDL_TRUE);
  }

int lh_SDL_StartTextInput(lua_State *L) {
  SDL_StartTextInput();
  return 0;
  }

int lh_SDL_StopTextInput(lua_State *L) {
  SDL_StopTextInput();
  return 0;
  }
#endif

}
#endif
