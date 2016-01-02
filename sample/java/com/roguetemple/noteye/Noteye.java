package com.roguetemple.noteye;

public class Noteye {

    public static Runnable callback;
    public static Runnable errorhandler;

    public static int error_id, error_param;
    public static String error_b1, error_b2;

    public static final int dx[] = new int[] {1, 1, 0, -1, -1, -1, 0, 1};
    public static final int dy[] = new int[] {0, -1, -1, -1, 0, 1, 1, 1};
    public static final int DBASE = 512 * 6;
    public static final int DWAIT = 512 * 6 + 8;
    public static final int KEY_F0 = 256;
    public static final int NOTEYEERR = -256;

    static {
      System.loadLibrary("noteye");
      }

    public static native void init();
    public static native void halt();
    public static native void args(int argc, String[] argv);
    public static native void run(String noemain, boolean applyenv);
    public static native void finishinternal(int exitcode);
    public static native void uifinish();
    
    public static native void move(int y, int x);
    public static native void addch(char c);
    public static native void addstr(String s);
    public static native void mvaddch(int y, int x, char c);
    public static native void mvaddstr(int y, int x, String s);
    public static native void setTextAttr(int fore, int back);
    public static native void setTextAttr32(int fore, int back);
    public static native void noteye_curs_set(int i);
    public static native void noteye_curs_setx(int i);

    public static native int getticks();
    
    public static native int getch();
    public static native int getchev();

    // get details of the event returned by getchev
    // note that SDL_TEXTINPUT events do not work like in SDL
    // (they are broken down into separated characters)
    
    public static native int ev_type();
    public static native int ev_key();
    public static native int ev_scancode();
    public static native int ev_mods();

    public static native void refresh();
    public static native void noteye_halfdelay(int i);
    public static native void noteye_halfdelayms(int i);
    public static native void noteye_cbreak();
    
    public static native void globalstr(String name, String str);
    
    public static native int argcount();
    public static native int argint(int index);
    public static native double argnum(int index);
    public static native String argstr(int index);
    
    public static native void retint(int value);
    public static native void retbool(boolean value);
    public static native void retstr(String value);

    public static native void table_new();
    public static native void table_closesub();
    public static native void table_setInt(String name, int val);
    public static native void table_setStr(String name, String val);
    public static native void table_setBool(String name, boolean val);
    public static native void table_opensub(String name);

    public static native void table_setIntAtInt(int name, int val);
    public static native void table_setStrAtInt(int name, String val);
    public static native void table_setBoolAtInt(int name, boolean val);
    public static native void table_opensubAtInt(int name);
    
    public static void callJava() {
        callback.run();
        }

    public static void callErrorHandler(int id, String b1, String b2, int param) {
        error_id = id;
        error_b1 = b1;
        error_b2 = b2;
        error_param = param;
        errorhandler.run();
        }

    }
