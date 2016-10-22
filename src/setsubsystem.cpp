// a tool to change the subsystem information for a Windows executable
// necessary if we want to run a normally graphical NotEye game on its
// parent console
// (C) Zeno Rogue 2015-2016

#include <windows.h>
#include <winnt.h>
#include <stdio.h>

FILE *f;

_IMAGE_DOS_HEADER hdr;
_IMAGE_FILE_HEADER hdr2;
_IMAGE_OPTIONAL_HEADER hdr3;

int main(int argc, char **argv) { 
  if(argc != 3) {
    printf("Usage: setsubsystem <exec> <value>\n");
    printf("Value is 2 for Windows GUI, 3 for Windows console\n");
    return 0;
    }
  f = fopen(argv[1], "rb+");
  fread(&hdr, sizeof(hdr), 1, f);
  fseek(f, hdr.e_lfanew+4, SEEK_SET);
  fread(&hdr2, sizeof(hdr2), 1, f);
  fread(&hdr3, sizeof(hdr3), 1, f);
  printf("current subsystem = %d\n", hdr3.Subsystem);  
  hdr3.Subsystem = atoi(argv[2]);
  fseek(f, hdr.e_lfanew+4, SEEK_SET);
  fwrite(&hdr2, sizeof(hdr2), 1, f);
  fwrite(&hdr3, sizeof(hdr3), 1, f);
  fclose(f);
  return 0;
  }
