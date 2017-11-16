// Necklace of the Eye v3.0
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#include <windows.h>   

namespace noteye {

struct WinProcess : Process {

  CONSOLE_SCREEN_BUFFER_INFO csbinfo;
  CONSOLE_CURSOR_INFO ccinfo;

  HANDLE hStdout, hNewScreenBuffer, hConIn; 

  BOOL fSuccess; 
  
  int xsize, ysize;

  void addKeyEvent(const KEY_EVENT_RECORD& krec) {
    INPUT_RECORD irec;
    irec.Event.KeyEvent = krec;
    irec.EventType = KEY_EVENT;
    DWORD qty;
    WriteConsoleInput(hConIn, &irec, 1, &qty);
    }

  CHAR_INFO chi[8000];

  CHAR_INFO zero;

  CHAR_INFO& charAt(int x, int y) {
    return chi[y*xsize+x];
    }
  
  int getCursorSize() {
    if(!ccinfo.bVisible) return 0;
    return ccinfo.dwSize;
    }

  int copyScreen() {
  
    SMALL_RECT srctReadRect; 
    COORD coordBufSize; 
    COORD coordBufCoord; 
  
    GetConsoleScreenBufferInfo(hNewScreenBuffer, &csbinfo);
    GetConsoleCursorInfo(hNewScreenBuffer, &ccinfo);
      
    if(csbinfo.dwSize.X != xsize || csbinfo.dwSize.Y != ysize) {
      xsize = csbinfo.dwSize.X;
      ysize = csbinfo.dwSize.Y;
      }
      
    if(xsize * ysize > 8000) return false;
  
    srctReadRect.Top = 0;
    srctReadRect.Left = 0; 
    srctReadRect.Bottom = ysize-1;
    srctReadRect.Right = xsize-1; 
   
    coordBufSize.Y = ysize;
    coordBufSize.X = xsize;  
    coordBufCoord.X = 0; 
    coordBufCoord.Y = 0; 
   
    fSuccess = ReadConsoleOutput( 
      hNewScreenBuffer,        
      chi,      
      coordBufSize,   
      coordBufCoord,  
      &srctReadRect
      ); 
    
    if (! fSuccess) {
      noteyeError(6, "ReadConsoleOutput failed", NULL, GetLastError()); 
      return 0;
      }
      
    int curxl = curx; int curyl = cury;
    
    curx = csbinfo.dwCursorPosition.X;
    cury = csbinfo.dwCursorPosition.Y;
    
    int count = 0;
    if(curx != curxl) count++;
    if(cury != curyl) count++;
    
    for(int y=0; y<ysize; y++)
    for(int x=0; x<xsize; x++) {
      CHAR_INFO& nc = charAt(x,y);

      int C = addMerge(
        addFill(vgacol[nc.Attributes >> 4], 0xFFFFFF),
        addRecolor(f->gettile((unsigned char) nc.Char.AsciiChar), vgacol[nc.Attributes & 15], recDefault),
        false
        );

      int &OC = s->get(x,y);

      if(C != OC) {
        OC = C;
        count++;
        }
      }
    
    return count;
    }
  
  KEY_EVENT_RECORD ksave;
  char savedkey;
  
  void sendText(const string& s) { 
    // todo: sendText Windows
    for(int i=0; i<size(s); i++) {
      ksave.uChar.UnicodeChar = savedkey = s[i];
      addKeyEvent(ksave);
      }
    }
  
  void sendKey(int scancode, int keycode, int mod, bool down) {
    
    KEY_EVENT_RECORD krec;
    krec.bKeyDown = down;
    krec.wRepeatCount = 1;
    DWORD& cks(krec.dwControlKeyState);
    CHAR& ac(krec.uChar.AsciiChar);
    WORD& vkc(krec.wVirtualKeyCode);
    WORD& vsc(krec.wVirtualScanCode);
    
    krec.uChar.UnicodeChar = 0;    
    vsc = scancode;
    vkc = 0;
    cks = 0;
    
    int sym = keycode;
    ac = 0;
    
    if(mod & KMOD_LALT)  cks |= LEFT_ALT_PRESSED;
    if(mod & KMOD_RALT)  cks |= RIGHT_ALT_PRESSED;
    if(mod & KMOD_LCTRL) cks |= LEFT_CTRL_PRESSED;
    if(mod & KMOD_RCTRL) cks |= RIGHT_CTRL_PRESSED;
    if(mod & (KMOD_LSHIFT|KMOD_RSHIFT)) cks |= SHIFT_PRESSED;
    
    if(1) {

#define TRANS4(x,y,sc) if(sym == SDLK_##x) vkc = VK_##y, vsc = sc&255, cks |= (sc&256);

#define TRANS2(x,y)    TRANS4(x,y,0)
#define TRANS3(x,y,sc) TRANS4(x,y,sc)
#define TRANS(x)       TRANS4(x,x,0)
#define TRANSS(x,sc)   TRANS4(x,x,sc)

#define VK_0 0

      TRANSS(LEFT,75+256) TRANSS(RIGHT,77+256) TRANSS(UP,72+256) TRANSS(DOWN,80+256)

      TRANS(ESCAPE) TRANS(TAB)
      TRANS2(LSHIFT, SHIFT)
      TRANS2(RSHIFT, SHIFT)
      TRANS2(LCTRL, LCONTROL) 
      TRANS2(RCTRL, RCONTROL)
      TRANS2(LALT, 0)
      TRANS2(RALT, 0)
      TRANS(PAUSE)
      
      TRANS(F1) TRANS(F2) TRANS(F3) TRANS(F4) TRANS(F5)
      TRANS(F6) TRANS(F7) TRANS(F8) TRANS(F9) TRANS(F10)
      
      // TRANS(ALT) 
      
      TRANS2(BACKSPACE,BACK)
      TRANS3(RETURN,RETURN,28)
      
      TRANS3(PAGEUP, PRIOR,73+256)
      TRANS3(PAGEDOWN, NEXT,81+256)
      TRANS3(HOME, HOME, 71+256)
      TRANS3(END, END, 79+256)
      
      }

#ifdef SDL2
    int dirkeys[8] = { SDLK_KP_6, SDLK_KP_9, SDLK_KP_8, SDLK_KP_7, SDLK_KP_4, SDLK_KP_1, SDLK_KP_2, SDLK_KP_3 };
#else
    int dirkeys[8] = { SDLK_KP6, SDLK_KP9, SDLK_KP8, SDLK_KP7, SDLK_KP4, SDLK_KP1, SDLK_KP2, SDLK_KP3 };
#endif
    
    for(int i=0; i<8; i++) if(sym == dirkeys[i]) {

      // int dirssc[] = { 'M', 'I', 'H', 'G', 'K', 'O', 'P', 'Q' };
      
      /* numlock on
      int dirvkn[] = { VK_NUMPAD6, VK_NUMPAD9, VK_NUMPAD8, VK_NUMPAD7,
        VK_NUMPAD4, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3
        };

      char dirsnum[] = "69874123";

      ac = dirsnum[i], vkc = dirvkn[i], vsc = dirssc[i];
      */

      int dirvkn[] = { 39, 33, 38, 36, 37, 35, 40, 34};

      vkc = dirvkn[i];
      // vsc = dirssc[i];
      }
        
    if(vkc == 0) {
      if(sym >= 'A' && sym <= 'Z')
        vkc = sym;
      else if(sym >= '0' && sym <= '9')
        vkc = sym;
      else if(sym >= 'a' && sym <= 'z')
        vkc = sym - 32;
      else if(sym == '-') vkc = 189;
      else if(sym == '`') vkc = 192;
      else if(sym == '&') vkc = 166;
      else if(sym == '*') vkc = 56;
      else if(sym == '(') vkc = 57;
      else if(sym == ')') vkc = 48;
      else if(sym == '+') vkc = 187;
      else if(sym == '\'') vkc = 222;
      else if(sym == ',') vkc = 188;
      else if(sym == '.') vkc = 190;
      else if(sym == '/') vkc = 191;
      else if(sym == '|') vkc = 252;
      else vkc = sym + 128;
      }
    
    if(sym > 0 && sym < 32) {
      ac = sym;
      }

    if(sym >= 'a' && sym <= 'z' && (mod & (KMOD_LCTRL|KMOD_RCTRL))) {
      ac = sym - 96;
      }

    if(keycode >= 32 && keycode <= 255 && !(mod & (KMOD_LCTRL|KMOD_RCTRL))) {
      // wait for the sendtext event
      if(down) {
        ksave = krec;
        return;
        }
      else ac = savedkey;
      }
    
    addKeyEvent(krec);
    }

  void sendClick(int x, int y, int button) { };  // TODO implement
  
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  int startProcess() {
  
      ZeroMemory( &si, sizeof(si) );
      si.cb = sizeof(si);
      ZeroMemory( &pi, sizeof(pi) );
      
      logfile && fprintf(logfile, "Trying to call: %s\n", cmdline);
  
      // Start the child process. 
      if( !CreateProcess( NULL,   // No module name (use command line)
          (CHAR*) cmdline,        // Command line
          NULL,                   // Process handle not inheritable
          NULL,                   // Thread handle not inheritable
          TRUE,                   // Set handle inheritance to TRUE
          0,                      // No creation flags
          NULL,                   // Use parent's environment block
          NULL,                   // Use parent's starting directory 
          &si,                    // Pointer to STARTUPINFO structure
          &pi )                   // Pointer to PROCESS_INFORMATION structure
      ) 
      {
          noteyeError(7, "CreateProcess failed", NULL, GetLastError());
          return -1;
      }
  
    return 0;
    }
  
  int initProcess() {
  
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;
    
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
  
    if(xsize != -1 && ysize != -1) {
      SMALL_RECT rect = {0, 0, xsize-1, ysize-1};
      COORD bufSize = {xsize, ysize};
      int err = SetConsoleScreenBufferSize(hStdout, bufSize);
      // printf("(%d,%d), err = %d : %d\n", xsize, ysize, err, GetLastError());
      err = SetConsoleWindowInfo(hStdout, TRUE, &rect);
      // printf("(%d,%d), err = %d : %d\n", xsize, ysize, err, GetLastError());
      xsize = -1; ysize = -1;
      }
      
    hNewScreenBuffer = CreateConsoleScreenBuffer( 
       GENERIC_READ |           
       GENERIC_WRITE, 
       FILE_SHARE_READ | 
       FILE_SHARE_WRITE,        
       &sa,                      
       CONSOLE_TEXTMODE_BUFFER, 
       NULL);                   
    if (hStdout == INVALID_HANDLE_VALUE || 
            hNewScreenBuffer == INVALID_HANDLE_VALUE) 
    {
        logfile &&
        fprintf(logfile, "CreateConsoleScreenBuffer failed - (%d)\n", GetLastError()); 
        return 0;
    }
  
    // Make the new screen buffer the active screen buffer. 
  
    if (! SetConsoleActiveScreenBuffer(hNewScreenBuffer) ) 
    {
        logfile &&
        fprintf(logfile, "SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError()); 
        return 0;
    }
    
    int err  = SetStdHandle(STD_OUTPUT_HANDLE, hNewScreenBuffer);
    int err2 = SetStdHandle(STD_ERROR_HANDLE, hNewScreenBuffer);
    
    hConIn = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);
    
    /*
    char *enterMessage = "Hello world!\n\n";    
    DWORD writ;
    WriteFile(hNewScreenBuffer, enterMessage, strlen(enterMessage), &writ, NULL);
    */
    
    isActive = true;
    startProcess();
    return 1;
    }
  
  ~WinProcess() {
    if (! SetConsoleActiveScreenBuffer(hStdout)) 
    {
        logfile &&
        fprintf(logfile, "SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError()); 
    }
  
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    }

#ifdef USELUA
  bool checkEvent(lua_State *L) {
    if(!isActive) return false;
    
    int val = WaitForSingleObject( pi.hProcess, 25);
    if(val == 0) {
      isActive = false;
      lua_newtable(L);
      noteye_table_setInt(L, "type", evProcQuit);
      noteye_table_setInt(L, "obj", id);
      noteye_table_setInt(L, "exitcode", 0);
      return true;
      }

    if(copyScreen()) {
      lua_newtable(L);
      noteye_table_setInt(L, "type", evProcScreen);
      noteye_table_setInt(L, "obj", id);
      return true;
      }
    
    return false;
    }
#endif

  WinProcess(Screen *scr, Font *font, const char *cmdline) : Process(scr, font, cmdline) { 
    initProcess();
    }
  };

Process *startProcess(Screen *scr, Font *f, const char *cmdline) {
  return new WinProcess(scr, f, cmdline);
  }

}
