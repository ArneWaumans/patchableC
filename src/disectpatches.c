#include "disectpatches.h"

static patchData* disectPatches(char **patchLines, size_t lineAmount)
{
  patchData *patches = malloc(sizeof(patchData) * 5);

  for (int i = 0; i < lineAmount; i++)
  {
    printf("%s", patchLines[i]);
  }

  return patches;
}

static patchData* getPatchComps(char *patchFilePath)
{
  FILE *patchFile = fopen(patchFilePath, "r");
  if (patchFile == NULL)
  {
    perror("unable to open file");
    exit(EXIT_FAILURE);
  }
  
  char chunk[128];
  size_t len = sizeof(chunk);
  char *line = malloc(len * sizeof(char));
  if (line == NULL)
  {
    perror("unable to allocate memory");
    exit(EXIT_FAILURE);
  }
  line[0] = '\0';

  unsigned int allocatedLines = 40;
  char **textFromFile = malloc(sizeof(char*) * allocatedLines);
  if (textFromFile == NULL)
  {
    perror("unable to allocate memory");
    exit(EXIT_FAILURE);
  }
  unsigned int iterations = 0;

  while(fgets(chunk, sizeof(chunk), patchFile) != NULL)
  {
    size_t len_used = strlen(line);
    size_t chunk_used = strlen(chunk);

    if (len - len_used < chunk_used)
    {
      len *= 2;
      if ((line = realloc(line, len)) == NULL)
      {
        perror("Unable to reallocate memory");
        free(line);
        exit(1);
      }
    }

    strncpy(line + len_used, chunk, len - len_used);
    len_used += chunk_used;

    if (line[len_used - 1] == '\n')
    {
      if (iterations >= allocatedLines)
      {
        allocatedLines += 40;
        if ((textFromFile = (char**)realloc(textFromFile,sizeof(char*) * allocatedLines)) == NULL)
        {
          perror("reallocation failed\n");
          exit(1);
        }
      }
      textFromFile[iterations] = (char*)malloc(strlen(line) * sizeof(char));
      strcpy(textFromFile[iterations], line);
      line[0] = '\0';
      iterations++;
    }
  }
  patchData *patches = disectPatches(textFromFile, iterations);

  fclose(patchFile);
  free(line);
  for(int i = 0; i < iterations; i++)
    free(textFromFile[i]);
  free(textFromFile);

  return NULL;
}

void createPatchFiles(char **patchFilePaths)
{
  getPatchComps("/home/arne/code/cproj/patchableC/patches/surf-2.0-homepage.diff");
}
