#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "getpatchnames.h"
#include "disectpatches.h"
#include "applypatches.h"

int main()
{
  patches patchFiles = getPatchFiles();
  
  for (int i = 0; i < patchFiles.numPatchFiles; i++)
    printf("patch file: %s\n", patchFiles.patches[i]);
 
  printf("total patches: %d\n", patchFiles.numPatchFiles);

  freePatchFiles(&patchFiles);
}
