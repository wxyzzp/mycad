#include "StdAfx.h"
#include "DwfEventReactor.h"
#include "EmbeddedImageDef.h"

//ODRX_CONS_DEFINE_MEMBERS(DwfEventReactor, OdRxEventReactor, RXIMPL_CONSTR)

DwfEventReactor::DwfEventReactor() : _db(0)
{
}

void DwfEventReactor::beginSave(OdDbDatabase* db, const OdChar* pIntendedName)
{
  if (_db != db) return;
  for (unsigned int i = 0; i < _images.size(); i++)
  {
    _images[i]->beginSave(pIntendedName);
  }
}
void DwfEventReactor::addImage(EmbeddedImageDefPtr im)
{
  _images.push_back(im);
}

void DwfEventReactor::databaseToBeDestroyed(OdDbDatabase* db)
{
  if (db != _db) return;
  OdRxEventPtr evt = odrxSysRegistry()->getAt(ODRX_EVENT_OBJ);
  evt->removeReactor(this);
  ReactorMap::iterator i = _reactorMap.find(db);
  ODA_ASSERT(i != _reactorMap.end());
  _reactorMap.erase(i);
}

DwfEventReactor::ReactorMap DwfEventReactor::_reactorMap;

DwfEventReactor* DwfEventReactor::getReactor(OdDbDatabase* db)
{
  ReactorMap::iterator i = _reactorMap.find(db);
  if ( i != _reactorMap.end() ) return i->second;
  OdRxEventReactorPtr r = DwfEventReactor::createObject();
  OdRxEventPtr evt = odrxSysRegistry()->getAt(ODRX_EVENT_OBJ);
  evt->addReactor(r);
  DwfEventReactor* der = static_cast<DwfEventReactor*>( r.get() );
  der->_db = db;
  _reactorMap[db] = der;
  return der;
}

