const DBASE = 1024*3;

const D_RIGHT = (DBASE+0);
const D_UP    = (DBASE+2);
const D_LEFT  = (DBASE+4);
const D_DOWN  = (DBASE+6);
const D_PGUP  = (DBASE+1);
const D_PGDN  = (DBASE+7);
const D_HOME  = (DBASE+3);
const D_END   = (DBASE+5);
const D_CTR   = (DBASE+8);

extern (C)
{

void noteye_args(int argc, char **argv);
void noteye_init();
void noteye_run(const char *noemain, bool applyenv);
void noteye_halt();
void noteye_uiresume();
void noteye_uifinish();
void noteye_finishinternal(int exitcode);
void noteye_globalstr(const char *name, const char *value);

int noteye_getinternalx();
int noteye_getinternaly();

struct lua_State;
alias luafunc = int function(lua_State* L);
void noteye_globalfun(const char *name, luafunc f);

alias NoteyeErrorHandler = void function(int id, const char *b1, const char *b2, int param);
void noteye_handleerror(NoteyeErrorHandler h);

int noteye_getch();
void noteye_move(int y, int x);
void noteye_addch(char ch);
void noteye_addchx(int ch);
void noteye_addstr(const char *buf);
int noteye_inch();
int noteye_mvinch(int y, int x);
void noteye_erase();
void noteye_clrtoeol();
void noteye_refresh();
void setTextAttr(int fore, int back);
void setTextAttr32(int fore, int back);

int noteye_table_new (lua_State *L);
void noteye_table_opensub (lua_State *L, const char *index);
void noteye_table_opensubAtInt (lua_State *L, int index);
void noteye_table_closesub (lua_State *L);

void noteye_table_setInt   (lua_State *L, const char *index, int value);
void noteye_table_setNum   (lua_State *L, const char *index, double value);
void noteye_table_setStr   (lua_State *L, const char *index, const char *s);
void noteye_table_setBool  (lua_State *L, const char *index, bool b);
void noteye_table_setIntAtInt   (lua_State *L, int index, int value);
void noteye_table_setNumAtInt   (lua_State *L, int index, double value);
void noteye_table_setStrAtInt   (lua_State *L, int index, const char *s);
void noteye_table_setBoolAtInt  (lua_State *L, int index, bool b);

int noteye_argInt(lua_State *L, int);
real noteye_argNum(lua_State *L, int);
bool noteye_argBool(lua_State *L, int);
char *noteye_argStr(lua_State *L, int);

int noteye_retInt(lua_State *L, int i);
int noteye_retStr(lua_State *L, const char *s);
int noteye_retBool(lua_State *L, bool b);

}
