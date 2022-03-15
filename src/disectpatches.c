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
  // every location in a patchfile is between "-" and ","
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
  
  // the name of the filename is between "/" and " "
  cpyNameStringPtr = strtok(cpyNameStringPtr, "/");
  cpyNameStringPtr = strtok(NULL, " ");
  strcpy(data->filename, cpyNameStringPtr);
}

static arrPatchData disectPatches(char **patchLines, size_t lineAmount)
{
  // initialize patch data
  const unsigned int startPatchAmount = 8;
  unsigned int patchAmount = startPatchAmount;
  patchData *patches = malloc(sizeof(patchData) * startPatchAmount);
  unsigned int currentPatch = 0;

  unsigned int currentTextLine = 1;
  bool firstCommit = true;
  // necessary for algorithm
  int prevDiffCount = 0;

  // can be the same for multiple pathces
  char currentFileName[MAXBUF];
  int currentConfigLines[2] = {0, 0};

  for (int i = 0; i < lineAmount; i++)
  {
    printf("%d: %s", currentTextLine, patchLines[i]);

    // check if the current line contains important data
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
          // reallocate extra memory if necessary
          patchAmount += 5;
          patches = realloc(patches, sizeof(patchData) * patchAmount);
        }
        currentPatch++;
      }
      firstCommit = false;

      // prepare data for getNameFromString() and set the filename
      memset(currentFileName, '\0', sizeof(currentFileName));
      strcpy(currentFileName, patchLines[i]);
      getNameFromString(currentFileName, &patches[currentPatch]);

      // format check, some patches contain an index line
      currentConfigLines[0] = currentTextLine;
      if (strncmp("index", patchLines[i+1], strlen(index)) == 0)
        currentConfigLines[1] = currentTextLine + 3;
      else
        currentConfigLines[1] = currentTextLine + 2;
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
          // reallocate extra memory if necessary
          patchAmount += 5;
          patches = realloc(patches, sizeof(patchData) * patchAmount);
        }
        currentPatch++;
      }

      // prepare data for getNumFromString() and set the line number
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

  arrPatchData result = {.data = patches, .amount = currentPatch + 1};
  return result;
}

static arrPatchData getPatchComps(char *patchFilePath)
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
  arrPatchData patches = disectPatches(textFromFile, iterations);
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

void createPatchFiles(patches *patchFilePaths)
{
  // patch array that is going to hold all our patch data and the amount
  arrPatchData allPatches = {.data = NULL, .amount = 0};
  
  for (int i = 0; i < patchFilePaths->numPatchFiles; i++)
  {
    // check if directory is real
    if (patchFilePaths->patches[i][strlen(patchFilePaths->patches[i]) - 1] == '.')
      continue;
    
    printf("-----real dirs: %s-----\n", patchFilePaths->patches[i]);
    // get patches from current patch file
    arrPatchData patches = getPatchComps(patchFilePaths->patches[i]);
    
    // reallocate for extra patch data
    allPatches.data = (patchData*)realloc(allPatches.data, allPatches.amount + patches.amount);
    allPatches.amount += patches.amount;
    
    for (int j = 0; j < patches.amount; j++)
    {
      //printPatchData(&patches.data[j]);

      allPatches.data[allPatches.amount + j] = patches.data[j];
    }
    //freePatchData(patches.data);
  }

  for (int i = 0; i < allPatches.amount; i++)
    printPatchData(&allPatches.data[i]);
}
