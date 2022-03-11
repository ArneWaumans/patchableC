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

  while((read = getline(&line, &len, patchFile)) != NULL)
  {
    printf("length of line: %s\n", read);
    printf("text: %s\n", line);
  }
}

void createPatchFiles(char **patchFilePaths)
{
  
}