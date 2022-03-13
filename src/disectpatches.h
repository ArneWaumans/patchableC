#ifndef DISECT_PATCHES_H
#define DISECT_PATCHES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"

typedef struct patchData
{
  char patchname[50];
  char filename[50];
  int place;
  int configlines[2];
  int codelines[2];
} patchData;

void printPatchData(patchData* data);
static unsigned int getNumFromString(char *numString);
void getNameFromString(char *nameString, patchData *data);
static patchData* disectPatches(char **patchLines, size_t lineAmount);
static patchData* getPatchComps(char *patchFilePath);
void createPatchFiles(char **patchFilePaths);

#endif
