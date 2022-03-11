#include "disectpatches.h"

static patchData* getPatchComps(char *patchFilePath)
{
  char patchFileDir[MAXBUF];

  FILE *patchFile;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  patchFile = fopen(patchFilePath, "r");
  
  if (patchFile == NULL)
    exit(EXIT_FAILURE);

  /*while()*/ //help: https://solarianprogrammer.com/2019/04/03/c-programming-read-file-lines-fgets-getline-implement-portable-getline/
  /*{*/
    /*printf("length of line: %d\n", (int)read);*/
    /*printf("text: %s\n", line);*/
  /*}*/

  return NULL;
}

void createPatchFiles(char **patchFilePaths)
{
  getPatchComps("/home/arne/code/cproj/patchableC/patches/surf-2.0-homepage.diff");
}
