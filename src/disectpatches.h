#ifndef DISECT_PATCHES_H
#define DISECT_PATCHES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"

typedef struct patchData
{
  char patchname[MAXBUF];
  char filename[FILEBUF];
  int place;
  int configlines[2];
  int codelines[2];
} patchData;

typedef struct returnPatchData
{
  patchData *data;
  unsigned int amount;
} returnPatchData;

static unsigned int getNumFromString(char *numString);
static void getNameFromString(char *nameString, patchData *data);
static returnPatchData disectPatches(char **patchLines, size_t lineAmount);
static returnPatchData getPatchComps(char *patchFilePath);

void printPatchData(patchData *data);
void createPatchFiles(char **patchFilePaths);
void freePatchData(patchData *data);

#endif
