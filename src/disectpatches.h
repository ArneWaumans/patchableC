#ifndef DISECT_PATCHES_H
#define DISECT_PATCHES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

typedef struct patchData
{
  char* patchname;
  char* filename;
  int place;
  int configlines[2];
  int codelines[2];
} patchData;

static patchData* disectPatches(char **patchLines, size_t lineAmount);
static patchData* getPatchComps(char *patchFilePath);
void createPatchFiles(char **patchFilePaths);

#endif
