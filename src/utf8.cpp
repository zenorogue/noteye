// Necklace of the Eye v8.4
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

int utf8_numbytes(const char *s, int pos) {
  s += pos;
  int res = 0;
  for(int i = 7; i > 0; --i)
    if(*s & (1 << i)) res++;
      else break;
  if(res==0) return 1;
  return res;
  }

// int utf8_ofs[5] = {0, 0x80, 0x2080, 0x82080, 0x2082080};

int utf8_decode(const char *s, int pos) {
  int nb = 0;
  s += pos;
  for(int i = 7; i > 0; --i) {
    if(*s & (1 << i)) { nb++; continue; }
    int res = *s & ((1 << i) - 1);
    s++; nb--;
    // int ofs = utf8_ofs[nb];
    while(nb>0) {
      res <<= 6;
      res += (*s) & 63;
      s++; nb--;
      }
    return res;
    }
  return 0;
  }

struct myuchar {
  char c[7];
  char& operator[] (int i) { return c[i]; }
  };

myuchar utf8_encode_array(int i) {
  myuchar res;
  if(i < 0x80) { res[0] = i; res[1] = 0; return res; } // else i -= 0x80;
  if(i < 0x800) { res[0] = 192 + (i>>6); res[1] = 0x80 + (i & 63); res[2] = 0; return res; }
  // else i -= 0x2000;
  if(i < 0x10000) { res[0] = 224 + (i >> 12); res[1] = 0x80 + ((i>>6) & 63); res[2] = 0x80 + (i&63); res[3] = 0; return res; }
  // else i -= 0x80000;
  if(i < 0x200000) { 
    res[0] = 240 + (i >> 18); 
    res[1] = 0x80 + ((i>>12) & 63); 
    res[2] = 0x80 + ((i>>6)  & 63); 
    res[3] = 0x80 + ((i>>0)  & 63); 
    res[4] = 0;
    return res; 
    }
  if(i < 0x4000000) {
    res[0] = 248 + (i >> 24); 
    res[1] = 0x80 + ((i>>18) & 63); 
    res[2] = 0x80 + ((i>>12) & 63); 
    res[3] = 0x80 + ((i>>6)  & 63); 
    res[4] = 0x80 + ((i>>0)  & 63); 
    res[5] = 0;
    return res;
    }
  if(true) {
    res[0] = 252 + (i >> 30); 
    res[1] = 0x80 + ((i>>24) & 63); 
    res[2] = 0x80 + ((i>>18) & 63); 
    res[3] = 0x80 + ((i>>12) & 63); 
    res[4] = 0x80 + ((i>>6)  & 63); 
    res[5] = 0x80 + ((i>>0)  & 63); 
    res[6] = 0;
    return res;
    }
  return res; 
  }

const char *utf8_encode(int i) { static myuchar res = utf8_encode_array(i); return (char*) &res; }

int utf8_strlen(const char *s) { 
  int i = 0;
  while(*s) i++, s += utf8_numbytes(s);
  return i;
  }

string substr_tmp;

// uses Lua numbering
const char *utf8_substr(const char *s, int i, int j) { 
  substr_tmp = "";
  int pos = 1;
  while(*s) {
    int l = utf8_numbytes(s);
    if(pos >= i && pos <= j)
      while(l--) substr_tmp += *(s++);
    else s += l;
    pos++;
    }
  return substr_tmp.c_str();
  }

}
