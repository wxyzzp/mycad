#include <fstream.h>

#include "Gr.h"
#include "DoDef.h"
#include "matrix.h"

//** History
//** 	24-03-2003 Bugzilla#78471 Support Lineweight

/*DG - 7.7.2003*/// to use greedy_heap for alloc/free
#define USE_GREEDYHEAP_FOR_DO

#ifdef USE_GREEDYHEAP_FOR_DO
#include "greedyheap.h"
static icl::greedy_heap<gr_displayobject>	s_doHeap(offsetof(gr_displayobject, next));
#endif


GR_API gr_displayobject* copyDisplayObject(struct gr_displayobject *pSourceObject, const CMatrix4* pTransformation /* = NULL */)
{
	/*DG - 16.4.2003*/// I made it non-recursive.

	gr_displayobject	*pRetval = NULL,
						*pDO = NULL;
	int					ndims, iSize;

	for( ; pSourceObject; pSourceObject = pSourceObject->next)
	{
		if(pDO)
		{
#ifdef USE_GREEDYHEAP_FOR_DO
			pDO->next = s_doHeap.malloc();
#else
			pDO->next = new gr_displayobject;
#endif
			pDO = pDO->next;
		}
		else
#ifdef USE_GREEDYHEAP_FOR_DO
			pRetval = pDO = s_doHeap.malloc();
#else
			pRetval = pDO = new gr_displayobject;
#endif
		ASSERT(pDO);
		if(!pDO)
			break;
		else
		{
			pDO->type = pSourceObject->type;
			pDO->color = pSourceObject->color;
			pDO->npts = pSourceObject->npts;
			pDO->next = NULL;
			pDO->SetSourceEntity(pSourceObject->GetSourceEntity());

			ndims = ((pSourceObject->type & DISP_OBJ_PTS_3D) != 0) + 2,
			iSize = pSourceObject->npts * ndims;
			
			pDO->chain = new sds_real[iSize];
			if(!pTransformation)
				memcpy(pDO->chain, pSourceObject->chain, sizeof(sds_real) * iSize);
			else
				if(ndims == 2)
				{
					for(int i = 0; i < iSize; i += 2)
					{
						pDO->chain[i] = (*pTransformation)(0,0) * pSourceObject->chain[i] +
										(*pTransformation)(0,1) * pSourceObject->chain[i + 1] +
										(*pTransformation)(0,3);
						pDO->chain[i + 1] = (*pTransformation)(1,0) * pSourceObject->chain[i] +
											(*pTransformation)(1,1) * pSourceObject->chain[i + 1] +
											(*pTransformation)(1,3);
					}
				}
				else
				{
					for(int i = 0; i < iSize; i += 3)
						pTransformation->multiplyRight(&(pDO->chain[i]), &(pSourceObject->chain[i]));
				}
		}
	}

	return pRetval;
}

GR_API void gr_freedisplayobjectll(void* pList) /* Void * so db can use it. */
{  
#ifdef USE_GREEDYHEAP_FOR_DO
	for(gr_displayobject* pDo = (gr_displayobject*)pList; pDo; pDo = pDo->next)
		delete [] pDo->chain;
	s_doHeap.freel((gr_displayobject*)pList);
#else
    gr_displayobject* doll= reinterpret_cast<gr_displayobject*>(pList);
    while (doll)
    {
		gr_displayobject* tp = doll->next;
		gr_displayobject::freeDisplayObject(doll);
		doll = tp;
	}
#endif
	pList = NULL;
}

// ************************************************
// CLASS IMPLEMENTATION -- class gr_displayobject
//

// *************************
// CREATOR METHOD -- class gr_displayobject
// 
// This is a class method (aka static)
//
// gr_displayobject is a struct that started life without CTOR/DTOR
//
// This creator method replaces mallocs scattered through the code.
// Eventually this can go away and be replaced by new, but this allows
// an incremental approach without tracking down all the free's etc.
//
gr_displayobject* gr_displayobject::newDisplayObject(db_entity *pSourceEntity)
{
#ifdef USE_GREEDYHEAP_FOR_DO
	gr_displayobject*	pRetval = s_doHeap.malloc();
#else
	gr_displayobject *pRetval = new gr_displayobject;
#endif
	
	ASSERT( pRetval != NULL );
	if ( pRetval != NULL )
	{
		pRetval->next = NULL;
		pRetval->npts = 0;
		pRetval->chain = NULL;
		pRetval->color = 0;
		pRetval->lweight = 0;
		// TODO TODO TODO
		// Need to ASSERT validity, but can't right now because it isn't always valid
		// ASSERT( pSourceEntity->IsValid() );
		pRetval->m_pSourceEntity = pSourceEntity;
	}
	
	return pRetval;
}

// ***************************
// DESTROYER METHOD -- class gr_displayobject
//
// see newDisplayObject comments
//
// This frees ONE entity, not the whole chain
//
void gr_displayobject::freeDisplayObject(gr_displayobject* pObject)
{
    if(pObject != NULL) 
	{
        if(pObject->chain != NULL) 
			delete [] pObject->chain;
#ifdef USE_GREEDYHEAP_FOR_DO
		s_doHeap.free(pObject);
#else
		delete pObject; 
#endif
    }
}


ostream& operator<<(ostream &os, const gr_displayobject &dispObj)
{
	os << "Display Object Pointer: " << &dispObj << " {" << endl;
	os << "Color: " << dispObj.color << endl;
	os << "LWeight: " << dispObj.lweight << endl;
	os << "Number of points in the chain: " << dispObj.npts << endl;
	os << "Chain pointer: " << dispObj.chain << endl;
	if (dispObj.npts > 0 && !dispObj.chain) 
		os << "Null chain pointer" << endl;

	sds_real* pChain = dispObj.chain;
	for (register i = 0; i < dispObj.npts; i++) {
		os << "Point[" << i << "]: (" << *pChain++ << ", " 
		   << *pChain++ << ", " << *pChain++ << ")" << endl;
	}
	os << "Source entity: " << "dispObj.m_pSourceEntity" << endl;
	if (dispObj.m_pSourceEntity) {
		os << "Source entity type: " << dispObj.m_pSourceEntity->ret_type() 
		   << "; ACIS entity: " << dispObj.m_pSourceEntity->isAcisEntity() << endl;
	}
	os << "Next pointer: " << dispObj.next << " } " << endl;
	return os;
}



#ifdef _GR_DEBUG

void printDispObjList(ostream &os, gr_displayobject* pEntityListBegin, 
					               gr_displayobject* pEntityListEntity)
{
	os << "******** DISPLAY LIST(gr_displayobject*) BEGIN *************" << endl;
	gr_displayobject* pThisEntity = pEntityListBegin;
	if (pThisEntity) {
		do {
			os << *pThisEntity << endl;
			pThisEntity = pThisEntity->next;
		} while (pThisEntity != pEntityListEntity);
	}
	else
		os << "Error: Empty entity list." << endl;

	os << "******** DISPLAY LIST(gr_displayobject*) END *************" << endl;
}



void printDispObjList(ostream &os, sds_dobjll* pDispList) 
{
	os << "******** DISPLAY LIST(sds_dobjll) BEGIN *************" << endl;
	sds_dobjll* pThisObject = pDispList;
	do {
		os << "Display Object Pointer: " << pThisObject << " {" << endl;
		os << "Color: " << pThisObject->color << endl;
		os << "LWeight: " << pThisObject->lweight << endl;
		os << "Number of points in the chain: " << pThisObject->npts << endl;
		os << "Chain pointer: " << pThisObject->chain << endl;
		if (pThisObject->npts > 0 && !pThisObject->chain) 
			os << "Null chain pointer" << endl;

		sds_real* pChain = pThisObject->chain;
		for (register i = 0; i < pThisObject->npts; i++) {
			if( pThisObject->type & DISP_OBJ_PTS_3D ) {
				os << "Point[" << i << "]: (" << *pChain++ << ", " 
				<< *pChain++ << ", " << *pChain++ << ")" << endl;
			} else {
				os << "Point[" << i << "]: (" << *pChain++ << ", " 
				<< *pChain++ << ")" << endl;
			}
		}
		pThisObject = pThisObject->next;
		os << "Next pointer: " << pThisObject << " } " << endl;
	} while (pThisObject && pThisObject != pDispList);

	os << "******** DISPLAY LIST(sds_dobjll) END *************" << endl;
}


#endif // _GR_DEBUG

