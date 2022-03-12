#include "disectpatches.h"

void printPatchData(patchData* data)
{
  printf("-----patchdata-----\n");
  printf("patchname: %s\n", data->patchname);
  printf("filename: %s\n", data->filename);
  printf("place: %d\n", data->place);
  printf("configlines: [%d, %d]", data->configlines[0], data->configlines[1]);
  printf("codelines: [%d, %d]", data->codelines[0], data->codelines[1]);
}

static unsigned int getNumFromString(char* numString)
{
  numString = strtok(numString, "-");
  numString = strtok(NULL, ",");

  return atoi(numString);
}

void getNameFromString(char *nameString, patchData *data)
{
  nameString = strtok(nameString, "/");
  nameString = strtok(NULL, " ");
  memcpy(data->filename, nameString, strlen(nameString) * sizeof(char));
}

static patchData* disectPatches(char **patchLines, size_t lineAmount)
{
  patchData *patches = malloc(sizeof(patchData) * 5);
  unsigned int currentPatch = 0;

  unsigned int currentTextLine = 1;
  bool firstCommit = true;
  int prevDiffCount = 0;

  for (int i = 0; i < lineAmount; i++)
  {
    printf("%d: %s", currentTextLine, patchLines[i]);

    char *diffMatch = strstr(patchLines[i], "diff --git");

    char *atLine = strstr(patchLines[i], "@@");

    if (diffMatch != NULL)
    {
      printf("\033[0;32m");
      printf("%d: Found 'diff' Match!\n", currentTextLine);
      printf("\033[0m");

      prevDiffCount = 0;

      if (!firstCommit)
      {
        patches[currentPatch].codelines[1] = currentTextLine - 1;
        currentPatch++;
      }
      firstCommit = false;
     
      char extractName[150];
      memset(extractName, '\0', sizeof(extractName));
      strcpy(extractName, patchLines[i]);
      getNameFromString(extractName, &patches[currentPatch]);
      printf("filename: %s\n", patches[currentPatch].filename);

      patches[currentPatch].configlines[0] = currentTextLine;
      patches[currentPatch].configlines[1] = currentTextLine + 3;
    }

    if (atLine != NULL)
    {
      prevDiffCount++;

      if (prevDiffCount > 1)
      {
        patches[currentPatch].codelines[1] = currentTextLine - 1;
        currentPatch++;
      }
      
      char extractLineNumber[150];
      memset(extractLineNumber, '\0', sizeof(extractLineNumber));
      strcpy(extractLineNumber, patchLines[i]);
      patches[currentPatch].place = getNumFromString(extractLineNumber);
      printf("place: %d\n", patches[currentPatch].place);

      patches[currentPatch].codelines[0] = currentTextLine;
    }

    currentTextLine++;
  }
  patches[currentPatch].codelines[1] = currentTextLine - 1;

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
        exit(EXIT_FAILURE);
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
