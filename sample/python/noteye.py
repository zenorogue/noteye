import ctypes
from ctypes import c_int, c_double, c_bool, POINTER, CFUNCTYPE, c_char_p

# on Windows, use "..\..\noteye.dll" instead
lib = ctypes.CDLL("../../libnoteye.so")

init = lib.noteye_init
globalstr = lib.noteye_globalstr
uifinish = lib.noteye_uifinish
finishinternal = lib.noteye_finishinternal
halt = lib.noteye_halt
run = lib.noteye_run
getch = lib.noteye_getch
move = lib.noteye_move
addstr = lib.noteye_addstr
addch = lib.noteye_addch

settextattr = lib.setTextAttr
settextattr32 = lib.setTextAttr32

def getchr():
    return chr(getch())

NOTEYEERR = (-256)
KEY_F0 = 256
DBASE = (512*6)
DWAIT = (DBASE + 8)

dx8 = [1, 1, 0, -1, -1, -1, 0, 1]
dy8 = [0, -1, -1, -1, 0, 1, 1, 1]

def isdir(ch):
    return ch >= DBASE and ch < DBASE+8

def dxy(ch):
    return dx8[ch-DBASE], dy8[ch-DBASE]

LUASTATE = POINTER(c_int)

LUAFUN = CFUNCTYPE(c_int, LUASTATE)

ERRORHANDLER = CFUNCTYPE(None, c_int, c_char_p, c_char_p, c_int)

globalfun = lib.noteye_globalfun
globalfun.argtypes = [c_char_p, LUAFUN]
globalfun.restype = None

errorhandler = lib.noteye_handleerror
errorhandler.argtypes = [ERRORHANDLER]
errorhandler.restype = None

retInt = lib.noteye_retInt
retInt.argtypes = [LUASTATE, c_int]
retInt.restype = c_int

argint = lib.noteye_argInt
argint.argtypes = [LUASTATE, c_int]
argint.restype = c_int

table_new = lib.noteye_table_new
table_new.argtypes = [LUASTATE]
table_new.restype = c_int

table_opensub = lib.noteye_table_opensub
table_opensub.argtypes = [LUASTATE, c_char_p]
table_opensub.restype = None

table_opensubAtInt = lib.noteye_table_opensubAtInt
table_opensubAtInt.argtypes = [LUASTATE, c_int]
table_opensubAtInt.restype = None

table_closesub = lib.noteye_table_closesub
table_closesub.argtypes = [LUASTATE]
table_closesub.restype = None

table_setInt = lib.noteye_table_setInt
table_setInt.argtypes = [LUASTATE, c_char_p, c_int]
table_setInt.restype = None

table_setNum = lib.noteye_table_setNum
table_setNum.argtypes = [LUASTATE, c_char_p, c_double]
table_setNum.restype = None

table_setStr = lib.noteye_table_setStr
table_setStr.argtypes = [LUASTATE, c_char_p, c_char_p]
table_setStr.restype = None

table_setBool = lib.noteye_table_setBool
table_setBool.argtypes = [LUASTATE, c_char_p, c_bool]
table_setBool.restype = None

table_setIntAtInt = lib.noteye_table_setIntAtInt
table_setIntAtInt.argtypes = [LUASTATE, c_int, c_int]
table_setIntAtInt.restype = None

table_setNumAtInt = lib.noteye_table_setNumAtInt
table_setNumAtInt.argtypes = [LUASTATE, c_int, c_double]
table_setNumAtInt.restype = None

table_setStrAtInt = lib.noteye_table_setStrAtInt
table_setStrAtInt.argtypes = [LUASTATE, c_int, c_char_p]
table_setStrAtInt.restype = None

table_setBoolAtInt = lib.noteye_table_setBoolAtInt
table_setBoolAtInt.argtypes = [LUASTATE, c_int, c_bool]
table_setBoolAtInt.restype = None
