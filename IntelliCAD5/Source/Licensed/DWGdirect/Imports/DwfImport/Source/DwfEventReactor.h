#ifndef __DWFEVENTREACTOR_H
#define __DWFEVENTREACTOR_H

#include "EmbeddedImageDef.h"

class DwfEventReactor;

typedef OdSmartPtr<DwfEventReactor> DwfEventReactorPtr;

class DwfEventReactor : public OdRxEventReactor  
{
public:
  DwfEventReactor();
  static OdRxEventReactorPtr createObject()
  {
    return OdRxObjectImpl<DwfEventReactor>::createObject();
  }
  virtual void beginSave(OdDbDatabase*, const OdChar* pIntendedName);
  virtual void databaseToBeDestroyed(OdDbDatabase*);
  void addImage(EmbeddedImageDefPtr);
private:
  OdArray<EmbeddedImageDefPtr> _images;
  OdDbDatabase* _db;
  typedef std::map<OdDbDatabase*, DwfEventReactor*> ReactorMap;
  static ReactorMap _reactorMap;
public:
  static DwfEventReactor* getReactor(OdDbDatabase*);
};

#endif // __DWFEVENTREACTOR_H

