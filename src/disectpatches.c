#include "disectpatches.h"

void printPatchData(patchData *data)
{
  printf("-----patchdata-----\n");
  printf("patchname: %s\n", data->patchname);
  printf("filename: %s\n", data->filename);
  printf("place: %d\n", data->place);
  printf("configlines: [%d, %d]\n", data->configlines[0], data->configlines[1]);
  printf("codelines: [%d, %d]\n", data->codelines[0], data->codelines[1]);
  printf("\n");
}

static unsigned int getNumFromString(char *numString)
{
  numString = strtok(numString, "-");
  numString = strtok(NULL, ",");

  return atoi(numString);
}

void freePatchData(patchData *data)
{
  free(data);
}

static void getNameFromString(char *nameString, patchData *data)
{
  char cpyNameString[MAXBUF];
  memset(cpyNameString, '\0', sizeof(cpyNameString));
  char *cpyNameStringPtr = strcpy(cpyNameString, nameString);
  cpyNameStringPtr = strtok(cpyNameStringPtr, "/");
  cpyNameStringPtr = strtok(NULL, " ");
  strcpy(data->filename, cpyNameStringPtr);
}

static returnPatchData disectPatches(char **patchLines, size_t lineAmount)
{
  const unsigned int startPatchAmount = 8;
  unsigned int patchAmount = startPatchAmount;
  patchData *patches = malloc(sizeof(patchData) * startPatchAmount);
  unsigned int currentPatch = 0;

  unsigned int currentTextLine = 1;
  bool firstCommit = true;
  int prevDiffCount = 0;

  char currentFileName[MAXBUF];
  int currentConfigLines[2] = {0, 0};

  for (int i = 0; i < lineAmount; i++)
  {
    printf("%d: %s", currentTextLine, patchLines[i]);

    char *diffMatch = strstr(patchLines[i], "diff --git");
    char *atLine = strstr(patchLines[i], "@@");

    if (diffMatch != NULL)
    {
      prevDiffCount = 0;

      if (!firstCommit)
      {
        patches[currentPatch].codelines[1] = currentTextLine - 1;

        if (currentPatch >= patchAmount - 1)
        {
          patchAmount += 5;
          patches = realloc(patches, sizeof(patchData) * patchAmount);
        }
        currentPatch++;
      }
      firstCommit = false;
     
      memset(currentFileName, '\0', sizeof(currentFileName));
      strcpy(currentFileName, patchLines[i]);
      getNameFromString(currentFileName, &patches[currentPatch]);
      currentConfigLines[0] = currentTextLine;
      currentConfigLines[1] = currentTextLine + 3;
      memcpy(patches[currentPatch].configlines, currentConfigLines, sizeof(currentConfigLines));
    }

    if (atLine != NULL)
    {
      prevDiffCount++;

      if (prevDiffCount > 1)
      {
        patches[currentPatch].codelines[1] = currentTextLine - 1;
        getNameFromString(currentFileName, &patches[currentPatch]);
        memcpy(patches[currentPatch].configlines, currentConfigLines, sizeof(currentConfigLines));
        
        if (currentPatch >= patchAmount - 1)
        {
          patchAmount += 5;
          patches = realloc(patches, sizeof(patchData) * patchAmount);
        }
        currentPatch++;
      }
      
      char extractLineNumber[MAXBUF];
      memset(extractLineNumber, '\0', sizeof(extractLineNumber));
      strcpy(extractLineNumber, patchLines[i]);
      patches[currentPatch].place = getNumFromString(extractLineNumber);

      patches[currentPatch].codelines[0] = currentTextLine;
    }

    currentTextLine++;
  }
  getNameFromString(currentFileName, &patches[currentPatch]);
  memcpy(patches[currentPatch].configlines, currentConfigLines, sizeof(currentConfigLines));
  patches[currentPatch].codelines[1] = currentTextLine - 1;

  returnPatchData result = {.data = patches, .amount = currentPatch + 1};
  return result;
}

static returnPatchData getPatchComps(char *patchFilePath)
{
  FILE *patchFile = fopen(patchFilePath, "r");
  if (patchFile == NULL)
  {
    perror("unable to open file");
    exit(EXIT_FAILURE);
  }
  
  char chunk[MAXBUF];
  size_t len = sizeof(chunk);
  char *line = malloc(len * sizeof(char));
  if (line == NULL)
  {
    perror("unable to allocate memory");
    exit(EXIT_FAILURE);
  }
  line[0] = '\0';

  unsigned int allocatedLines = 100;
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
        allocatedLines += 50;
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
  returnPatchData patches = disectPatches(textFromFile, iterations);
  for (int i = 0; i < patches.amount; i++)
  {
    strcpy(patches.data[i].patchname, patchFilePath);
  }

  fclose(patchFile);
  free(line);
  for(int i = 0; i < iterations; i++)
    free(textFromFile[i]);
  free(textFromFile);

  return patches;
}

void createPatchFiles(char **patchFilePaths)
{
  returnPatchData patches = getPatchComps("/home/arne/code/cproj/patchableC/patches/surf-clipboard-20200112-a6a8878.diff");
  
  for (int i = 0; i < patches.amount; i++)
  {
    printPatchData(&patches.data[i]);
  }
}
