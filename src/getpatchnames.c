#include "getpatchnames.h"


void initPatchFiles(patches *patchObj, int preAlloc)
{
  patchObj->patches = malloc(preAlloc * sizeof(char*));
  patchObj->numPatchFiles = 0;
}

void freePatchFiles(patches *patchObj)
{
  for (int i = 0; i < patchObj->numPatchFiles; i++)
    free(patchObj->patches[i]);
  free(patchObj->patches);
}

patches getPatchFiles()
{
  int allocNumPatchFiles = 10;
  patches patchFiles;
  initPatchFiles(&patchFiles, allocNumPatchFiles);

  char patchDir[MAXBUF];
  getcwd(patchDir, MAXBUF);
  strcat(patchDir, "/patches");

  DIR *dir;
  dir = opendir(patchDir);
  struct dirent *ent;

  if (dir != NULL)
  {
    int i = 0;
    while((ent = readdir(dir)) != NULL)
    {
      patchFiles.numPatchFiles++;
      if (i+1 > allocNumPatchFiles)
      {
        patchFiles.patches = realloc(patchFiles.patches, allocNumPatchFiles + 5);
        allocNumPatchFiles += 5;
      }
      patchFiles.patches[i] = malloc((strlen(ent->d_name) + 1) * sizeof(char));
      strcpy(patchFiles.patches[i], ent->d_name);

      i++;
    }
  }

  return patchFiles;
}
