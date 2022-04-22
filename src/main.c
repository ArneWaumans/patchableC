#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "config.h"

#include "getpatchnames.h"
#include "disectpatches.h"
#include "applypatches.h"

static void setupFolders()
{
  char patchCompDir[MAXBUF];
  getcwd(patchCompDir, MAXBUF);
  strcat(patchCompDir, "/patchcomps");

  mkdir(patchCompDir, S_IRWXU);
}

static void removeFolders()
{
  char patchCompDir[MAXBUF];
  getcwd(patchCompDir, MAXBUF);
  strcat(patchCompDir, "/patchcomps");

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(patchCompDir)) != NULL)
  {
    while((ent = readdir(dir)) != NULL)
    {
      char patchCompFile[MAXBUF];
      strcpy(patchCompFile, patchCompDir);
      strcat(patchCompFile, "/");
      strcat(patchCompFile, ent->d_name);
      remove(patchCompFile);
    }
  }
  closedir(dir);

  rmdir(patchCompDir);
}

/* TODO: add funtionality to choose the directory from where to load the .diff files */
int main(int argc, char *argv[])
{   
  char* patchDir = malloc(sizeof(char) * MAXBUF);
  if (argc == 2)
  {
    strcpy(patchDir, argv[1]);
    printf("patch folder: %s\n", patchDir);
  }
  else if (argc > 2)
  {
    perror("Too many command line arguments");
    exit(-1);
  }
  else
  {
    patchDir = NULL;
  }
  
  setupFolders();

  patches *patchFiles = getPatchFiles(patchDir);
  
  for (int i = 0; i < patchFiles->numPatchFiles; i++)
    printf("patch file: %s\n", patchFiles->patches[i]);

  arrPatchData allPatches = createPatchFiles(patchFiles);

  applyPatches(allPatches);

  removeFolders();

  free(patchDir);

  freePatchFiles(patchFiles);
  freePatchData(allPatches.data);
}
