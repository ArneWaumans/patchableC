#ifndef GET_PATCH_NAMES_H
#define GET_PATCH_NAMES_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define MAXBUF 200

typedef struct patches
{
  int numPatchFiles;
  char **patches;
} patches;

void initPatchFiles(patches*, int);
void freePatchFiles(patches*);
patches getPatchFiles();

#endif
