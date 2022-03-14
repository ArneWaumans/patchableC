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

int main()
{
  setupFolders();

  patches *patchFiles = getPatchFiles();
  
  for (int i = 0; i < patchFiles->numPatchFiles; i++)
    printf("patch file: %s\n", patchFiles->patches[i]);

  createPatchFiles(patchFiles);

  freePatchFiles(patchFiles);
  removeFolders();
}
