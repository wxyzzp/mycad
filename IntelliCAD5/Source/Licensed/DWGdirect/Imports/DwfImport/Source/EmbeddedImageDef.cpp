#include "StdAfx.h"
#include "EmbeddedImageDef.h"

void EmbeddedImageDef::beginSave(const OdChar* pIntendedName)
{
  // extract base path
  OdString path = pIntendedName;
  // under windows in the path there may be both '\' and '/' separators
  {
    int i1 = path.reverseFind('\\');
    int i2 = path.reverseFind('/');
    int i = 0;
    if (i1 == -1)  i = i2;
    else if(i2 == -1) i = i1;
    else i = max(i1, i2);
    path = path.left(i+1);
  }
  // materialize image to the file by the path
  char filename[1024];
  int imageNum = 0;
  for (;;)
  {
    sprintf(filename, "%simage%d%s", (const char*)path, imageNum++, (const char*)_ext);
    if (FILE* f = fopen((const char*)filename, "r")) fclose(f);
    else break;
  }
  OdStreamBufPtr f = odSystemServices()->createFile(filename, Oda::kFileWrite);
  _data->rewind();
  _data->copyDataTo(f);

  // set this materialized file path to OdDbRasterImageDef
  upgradeOpen();
  setSourceFileName(filename);
  downgradeOpen();
}

