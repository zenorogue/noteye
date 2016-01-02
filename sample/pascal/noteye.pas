unit NotEye;

interface

type CInt = longint;

{ key IDs}

const NOTEYEERR = -256;

const KEY_F0  = 256;

const DBASE   = 3072;
const D_RIGHT = DBASE + 0;
const D_UP    = DBASE + 2;
const D_LEFT  = DBASE + 4;
const D_DOWN  = DBASE + 6;
const D_PGUP  = DBASE + 1;
const D_PGDN  = DBASE + 7;
const D_HOME  = DBASE + 3;
const D_END   = DBASE + 5;
const D_CTR   = DBASE + 8;

type TLuaState = record {irrelevant} end;
type PLuaState = ^TLuaState;
type ArgList = ^PChar;

const dx: Array[DBASE..DBASE+7] of Integer = (1, 1, 0, -1, -1, -1, 0, 1);
const dy: Array[DBASE..DBASE+7] of Integer = (0, -1, -1, -1, 0, 1, 1, 1);

type NoteyeLuaFunction = function(L: PLuaState): CInt; cdecl;

type NoteyeErrorHandler = procedure(id: CInt; b1, b2: PChar; param: CInt); cdecl;

procedure noteye_init; cdecl;
procedure noteye_halt; cdecl;
procedure noteye_run(s:PChar; applyenv: boolean); cdecl;
procedure noteye_args(argc: CInt; argv: ArgList); cdecl;
procedure noteye_handleerror(h: NoteyeErrorHandler); cdecl;

procedure noteye_globalstr(name:PChar; val:PChar); cdecl;
procedure noteye_globalint(name:PChar; val:CInt); cdecl;
procedure noteye_globalfun(name:PChar; val:NoteyeLuaFunction); cdecl;

procedure setTextAttr(fore, back: CInt); cdecl;
procedure noteye_move(y, x: CInt); cdecl;
procedure noteye_addch(ch: Char); cdecl;
procedure noteye_addstr(s: PChar); cdecl;

function noteye_getinternalx : CInt; cdecl;
function noteye_getinternaly : CInt; cdecl;
function noteye_getch : CInt; cdecl;

function noteye_retInt(L: PLuaState; i: CInt): CInt; cdecl;

procedure noteye_table_new      (L: PLuaState); cdecl;
procedure noteye_table_setInt   (L: PLuaState; index: PChar; val: CInt); cdecl;
procedure noteye_table_setNum   (L: PLuaState; index: PChar; val: Double); cdecl;
procedure noteye_table_setStr   (L: PLuaState; index: PChar; val: PChar); cdecl;
procedure noteye_table_setBool  (L: PLuaState; index: PChar; val: Boolean); cdecl;
procedure noteye_table_opensub  (L: PLuaState; index: PChar); cdecl;
procedure noteye_table_setIntAtInt   (L: PLuaState; index: CInt; val: CInt); cdecl;
procedure noteye_table_setNumAtInt   (L: PLuaState; index: CInt; val: double); cdecl;
procedure noteye_table_setStrAtInt   (L: PLuaState; index: CInt; val: PChar); cdecl;
procedure noteye_table_setBoolAtInt  (L: PLuaState; index: CInt; val: Boolean); cdecl;
procedure noteye_table_opensubAtInt  (L: PLuaState; index: CInt); cdecl;
procedure noteye_table_closesub (L: PLuaState); cdecl;

procedure noteye_finishinternal(exitcode: CInt); cdecl;
procedure noteye_uifinish(); cdecl;

implementation

{$linklib noteye}

procedure noteye_init; cdecl; external;
procedure noteye_halt; cdecl; external;
procedure noteye_run(s:PChar; applyenv: boolean); cdecl; external;
procedure noteye_args(argc: CInt; argv: ArgList); cdecl; external;
procedure noteye_handleerror(h: NoteyeErrorHandler); cdecl; external;

procedure noteye_globalstr(name:PChar; val:PChar); cdecl; external;
procedure noteye_globalint(name:PChar; val:CInt); cdecl; external;
procedure noteye_globalfun(name:PChar; val:NoteyeLuaFunction); cdecl; external;

procedure setTextAttr(fore, back: CInt); cdecl; external;
procedure noteye_move(y, x: CInt); cdecl; external;
procedure noteye_addch(ch: Char); cdecl; external;
procedure noteye_addstr(s: PChar); cdecl; external;

function noteye_getinternalx : CInt; cdecl; external;
function noteye_getinternaly : CInt; cdecl; external;
function noteye_getch : CInt; cdecl; external;

function noteye_retInt(L: PLuaState; i: CInt): CInt; cdecl; external;

procedure noteye_table_new      (L: PLuaState); cdecl; external;
procedure noteye_table_setInt   (L: PLuaState; index: PChar; val: CInt); cdecl; external;
procedure noteye_table_setNum   (L: PLuaState; index: PChar; val: Double); cdecl; external;
procedure noteye_table_setStr   (L: PLuaState; index: PChar; val: PChar); cdecl; external;
procedure noteye_table_setBool  (L: PLuaState; index: PChar; val: Boolean); cdecl; external;
procedure noteye_table_opensub  (L: PLuaState; index: PChar); cdecl; external;
procedure noteye_table_setIntAtInt   (L: PLuaState; index: CInt; val: CInt); cdecl; external;
procedure noteye_table_setNumAtInt   (L: PLuaState; index: CInt; val: double); cdecl; external;
procedure noteye_table_setStrAtInt   (L: PLuaState; index: CInt; val: PChar); cdecl; external;
procedure noteye_table_setBoolAtInt  (L: PLuaState; index: CInt; val: Boolean); cdecl; external;
procedure noteye_table_opensubAtInt  (L: PLuaState; index: CInt); cdecl; external;
procedure noteye_table_closesub (L: PLuaState); cdecl; external;

procedure noteye_finishinternal(exitcode: CInt); cdecl; external;
procedure noteye_uifinish(); cdecl; external;


end.
