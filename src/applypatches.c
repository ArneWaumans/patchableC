#include "applypatches.h"

void applyPatches(arrPatchData allPatches)
{
  for (int i = 0; i < allPatches.amount; i++)
    printPatchData(&allPatches.data[i]);

  FILE *fp;
  int status;
  char path[MAXBUF];

  fp = popen("ls", "r");
  if (fp == NULL)
    perror("cannot access pipe stream");

  while(fgets(path, MAXBUF, fp) != NULL)
    printf("%s", path);

  status = fclose(fp);
  if (status == -1)
    perror("cannot close pipe stream");
}
