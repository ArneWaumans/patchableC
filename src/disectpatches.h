#ifndef DISECT_PATCHES_H
#define DISECT_PATCHES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "config.h"
#include "getpatchnames.h"

typedef struct patchData
{
  char patchname[MAXBUF];
  char filename[FILEBUF];
  int place;
  // this contains the lines from "dif --git" to "+++ b/"
  int configlines[2];
  // this contains the lines from "@@ -" to the end
  int codelines[2];
} patchData;

typedef struct arrPatchData
{
  patchData *data;
  unsigned int amount;
} arrPatchData;

static unsigned int getNumFromString(char *numString);
static void getNameFromString(char *nameString, patchData *data);
static arrPatchData disectPatches(char **patchLines, size_t lineAmount);
static arrPatchData getPatchComps(char *patchFilePath);
static patchData* sortPatchData(arrPatchData *allPatches);

void printPatchData(patchData *data);
void createPatchFiles(patches *patchFilePaths);
void freePatchData(patchData *data);

#endif
