#include "noteye.h"
#include "noteye-curses.h"
#include "noteye-jni.h"

lua_State *JNIL;
JNIEnv *genv;
jclass noteyeclass;

using namespace noteye;

int retv;

int lua_jni(lua_State *L) { 
  retv = 0;
  JNIL = L;
  jmethodID mid = genv->GetStaticMethodID(noteyeclass, "callJava", "()V");
  
  genv->CallStaticVoidMethod(noteyeclass, mid);
  
  return retv;
  }

void jni_handler(int id, const char *b1, const char *b2, int param) { 
  jmethodID mid = genv->GetStaticMethodID(noteyeclass, "callErrorHandler", 
    "(ILjava/lang/String;Ljava/lang/String;I)V");
  
  jstring sb1 = genv->NewStringUTF(b1);
  jstring sb2 = genv->NewStringUTF(b2);
  
  genv->CallStaticVoidMethod(noteyeclass, mid, id, sb1, sb2, param);
  
  genv->DeleteLocalRef(sb1);
  genv->DeleteLocalRef(sb2);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_init
  (JNIEnv *env, jclass c) {
  noteye_init();
  noteye_globalfun("jni", lua_jni);
  noteye_handleerror(jni_handler);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_halt
  (JNIEnv *, jclass) {
  noteye_halt();
  }

struct jstringer {
  JNIEnv *_env;
  jstring orig;
  const char *s;
  jstringer(JNIEnv *env, jstring o) : _env(env), orig(o), s(env->GetStringUTFChars(o, 0)) {}
  ~jstringer() { _env->ReleaseStringUTFChars(orig, s); }
  };

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_run
  (JNIEnv *env, jclass cls, jstring s, jboolean b) {
  genv = env; noteyeclass = cls;
  jstringer _s(env, s);
  noteye_run(_s.s, b);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_globalstr
  (JNIEnv *env, jclass, jstring name, jstring val) {
  jstringer nname(env, name);
  jstringer nval(env, val);
  noteye_globalstr(nname.s, nval.s);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_setTextAttr
  (JNIEnv *, jclass, jint fore, jint back) {
  setTextAttr(fore, back);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_setTextAttr32
  (JNIEnv *, jclass, jint fore, jint back) {
  setTextAttr32(fore, back);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_move
  (JNIEnv *, jclass, jint y, jint x) {
  noteye_move(y, x);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_addch
  (JNIEnv *, jclass, jchar ch) {
  noteye_addch(ch);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    addstr
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_addstr
  (JNIEnv *, jclass, jstring s) {
  jstringer ns(genv, s);
  noteye_addstr(ns.s);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    mvaddch
 * Signature: (IIC)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_mvaddch
  (JNIEnv *, jclass, jint y, jint x, jchar ch) {
  noteye_mvaddch(y, x, ch);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    mvaddstr
 * Signature: (IILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_mvaddstr
  (JNIEnv *, jclass, jint y, jint x, jstring s) {
  jstringer ns(genv, s);
  noteye_mvaddstr(y, x, ns.s);
  }


JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_getch
  (JNIEnv *env, jclass cls) {
  genv = env; noteyeclass = cls; 
  return noteye_getch();
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_getticks
  (JNIEnv *env, jclass cls) {
  return SDL_GetTicks();
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_getchev
  (JNIEnv *env, jclass cls) {
  genv = env; noteyeclass = cls; 
  return noteye_getchev();
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_ev_1type
  (JNIEnv *env, jclass cls) {
  return noteye_getlastkeyevent()->type;
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_ev_1key
  (JNIEnv *env, jclass cls) {
  return noteye_getlastkeyevent()->key.keysym.sym;
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_ev_1scancode
  (JNIEnv *env, jclass cls) {
  return noteye_getlastkeyevent()->key.keysym.scancode;
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_ev_1mods
  (JNIEnv *env, jclass cls) {
  return noteye_getlastkeyevent()->key.keysym.mod;
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_argint
  (JNIEnv *, jclass, jint x) {
  return noteye_argInt(JNIL, x);
  }

JNIEXPORT jint JNICALL Java_com_roguetemple_noteye_Noteye_argcount
  (JNIEnv *, jclass) {
   return noteye_argcount(JNIL);
  }

JNIEXPORT jdouble JNICALL Java_com_roguetemple_noteye_Noteye_argnum
  (JNIEnv *, jclass, jint x) {
  return noteye_argNum(JNIL, x);
  }

JNIEXPORT jstring JNICALL Java_com_roguetemple_noteye_Noteye_argstr
  (JNIEnv *, jclass, jint x) {
  return genv->NewStringUTF(noteye_argStr(JNIL, x));
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_retint
  (JNIEnv *, jclass, jint x) {
  retv += noteye_retInt(JNIL, x);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_retbool
  (JNIEnv *, jclass, jboolean val) {
  retv += noteye_retBool(JNIL, val);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_retstr
  (JNIEnv *, jclass, jstring val) {
  jstringer nval(genv, val);
  retv += noteye_retStr(JNIL, nval.s);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1new
  (JNIEnv *, jclass) {
  retv++; noteye_table_new(JNIL);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_setInt
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1setInt
  (JNIEnv *, jclass, jstring index, jint val) {
  jstringer nindex(genv, index);
  noteye_table_setInt(JNIL, nindex.s, val);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_setStr
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1setStr
  (JNIEnv *, jclass, jstring index, jstring val) {
  jstringer nindex(genv, index);
  jstringer nval(genv, val);
  noteye_table_setStr(JNIL, nindex.s, nval.s);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_setBool
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1setBool
  (JNIEnv *, jclass, jstring index, jboolean val) {
  jstringer nindex(genv, index);
  noteye_table_setBool(JNIL, nindex.s, val);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_opensub
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1opensub
  (JNIEnv *, jclass, jstring index) {
  jstringer nindex(genv, index);
  noteye_table_opensub(JNIL, nindex.s);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_setIntAtInt
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1setIntAtInt
  (JNIEnv *, jclass, jint index, jint val) {
  noteye_table_setIntAtInt(JNIL, index, val);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_setStrAtInt
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1setStrAtInt
  (JNIEnv *, jclass, jint index, jstring val) {
  jstringer nval(genv, val);
  noteye_table_setStrAtInt(JNIL, index, nval.s);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_setBoolAtInt
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1setBoolAtInt
  (JNIEnv *, jclass, jint index, jboolean val) {
  noteye_table_setBoolAtInt(JNIL, index, val);
  }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    table_opensubAtInt
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1opensubAtInt
  (JNIEnv *, jclass, jint index) {
  noteye_table_opensubAtInt(JNIL, index);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_table_1closesub
  (JNIEnv *, jclass) {
  noteye_table_closesub(JNIL);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_finishinternal
  (JNIEnv *, jclass, jint x) {
  noteye_finishinternal(x);
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_uifinish
  (JNIEnv *, jclass) {
  noteye_uifinish();
  }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_refresh
  (JNIEnv *, jclass) { noteye_refresh(); }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    noteye_halfdelay
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_noteye_1halfdelay
  (JNIEnv *, jclass, jint x) { noteye_halfdelay(x); }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    noteye_halfdelayms
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_noteye_1halfdelayms
  (JNIEnv *, jclass, jint x) { noteye_halfdelayms(x); }

/*
 * Class:     com_roguetemple_noteye_Noteye
 * Method:    noteye_cbreak
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_noteye_1cbreak
  (JNIEnv *, jclass) { noteye_cbreak(); }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_noteye_1curs_1set
  (JNIEnv *, jclass, jint x) { noteye_curs_set(x); }

JNIEXPORT void JNICALL Java_com_roguetemple_noteye_Noteye_noteye_1curs_1setx
  (JNIEnv *, jclass, jint x) { noteye_curs_setx(x); }
