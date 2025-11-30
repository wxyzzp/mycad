#include "Icad.h"
#include "HatchChange.h"

//
// start of db_HatchChange Implementation
// *************************************************************************
db_HatchChange::db_HatchChange(ChangeType type, HatchChangeType hatchType) 
		: db_BinChange(type), m_paths(NULL), m_numPaths(0),
		  m_numPatLines(0), m_patLines(NULL), m_numSeeds(0), m_seeds(NULL)

{
	m_hatchType = hatchType;
}

db_HatchChange::~db_HatchChange() 
{
	if (m_paths) 
		delete [] m_paths;

	if (m_patLines)
		delete [] m_patLines;

	if (m_seeds)
		delete[] m_seeds;
}

bool db_HatchChange::CacheChangedData(struct db_BinChange **pBeginList, 
						struct db_BinChange **pCurrentNode, 
						void *oldval, 
						void *newval, 
						int len,
						HatchChangeType changeType)
{
	if (changeType == PARENT)
		return db_BinChange::CacheChangedData(pBeginList, pCurrentNode, oldval, newval, len);


	if(oldval==NULL || newval==NULL) 
		{
		return false;
		}

	db_HatchChange *pTempChangeNode = NULL;

	switch (changeType)
	{
	case BOUNDARY:
		{
			if (!compareHatchBoundaries((db_hatch *)oldval, (db_hatch *)newval))
			{
				pTempChangeNode = new db_HatchChange(db_BinChange::SUBOBJECT, changeType);
				VERIFY(pTempChangeNode->cacheHatchBoundaryChange((db_hatch *)oldval, (db_hatch *)newval));
			}
		}
		break;
	
	case PATTERN:
		{
			if (!comparePatternLines((db_hatch *)oldval, (db_hatch *)newval))
			{
				pTempChangeNode = new db_HatchChange(db_BinChange::SUBOBJECT, changeType);
				VERIFY(pTempChangeNode->cachePatternChange((db_hatch *)oldval, (db_hatch *)newval));
			}			
		}
		break;

	case SEED:
		{
			if (!compareSeeds((db_hatch *)oldval, (db_hatch *)newval))
			{
				pTempChangeNode = new db_HatchChange(db_BinChange::SUBOBJECT, changeType);
				VERIFY(pTempChangeNode->cacheSeedChange((db_hatch *)oldval, (db_hatch *)newval));
			}			
		}
		break;

	default:
		ASSERT(false); // not implemented yet.
		return false;
	}


	if (pTempChangeNode)
		AddNewLink(pBeginList, pCurrentNode, pTempChangeNode );

	return true;
}

bool db_HatchChange::compareHatchBoundaries(db_hatch *oldHatch, db_hatch *newHatch)
{
	int oldNumPaths, newNumPaths;
	oldHatch->get_91(&oldNumPaths);
	newHatch->get_91(&newNumPaths);

	if (oldNumPaths != newNumPaths)
		return false;

	for (int i = 0; i < oldNumPaths; i++)
	{
		if (oldHatch->ret_path(i) != newHatch->ret_path(i))
			return false;
	}

	return true;
}

bool db_HatchChange::comparePatternLines(db_hatch *oldHatch, db_hatch *newHatch)
{
	int oldNumLines, newNumLines;
	oldHatch->get_78(&oldNumLines);
	newHatch->get_78(&newNumLines);

	if (oldNumLines != newNumLines)
		return false;

	for (int i = 0; i < oldNumLines; i++)
	{
		if (oldHatch->ret_patline(i) != newHatch->ret_patline(i))
			return false;
	}

	return true;
}

bool db_HatchChange::compareSeeds(db_hatch *oldHatch, db_hatch *newHatch)
{
	int oldNumSeeds, newNumSeeds;
	oldHatch->get_98(&oldNumSeeds);
	newHatch->get_98(&newNumSeeds);

	if (oldNumSeeds != newNumSeeds)
		return false;

	sds_point oldSeed, newSeed;

	for (int i = 0; i < oldNumSeeds; i++)
	{
		oldHatch->get_seed(oldSeed, i);
		newHatch->get_seed(newSeed, i);

		// seed points are only 2D
		if ((oldSeed[0] != newSeed[0]) || (oldSeed[1] != newSeed[1]))
			return false;
	}

	return true;
}

bool db_HatchChange::cacheHatchBoundaryChange(db_hatch *oldHatch, db_hatch *newHatch)
{
	oldHatch->get_91(&m_numPaths);

	if (m_paths)
		delete [] m_paths;

	m_paths = new db_hatchpathlink[m_numPaths];

	for (int i = 0; i < m_numPaths; i++)
	{
		// no associations are copied with the path - associations need to be added
		// separately.
		m_paths[i] = *(oldHatch->ret_path(i));
		m_paths[i].numBoundObjs = oldHatch->ret_path(i)->numBoundObjs;

		if (m_paths[i].numBoundObjs > 0)
		{
			db_hatchboundobjslink *oldBoundObjs;
			oldHatch->get_pathboundobj(i, &oldBoundObjs);

			// just storing association info - don't need to add pointers
			m_paths[i].boundObjs = new db_hatchboundobjslink (*oldBoundObjs);
		}
	}

	return true;
}


bool db_HatchChange::cachePatternChange(db_hatch *oldHatch, db_hatch *newhatch)
{
	oldHatch->get_78(&m_numPatLines);

	// reallocate space
	if (m_patLines)
	{
		delete [] m_patLines;
		m_patLines = NULL;
	}

	if (m_numPatLines > 0)
	{
		m_patLines = new db_hatchpatlinelink[m_numPatLines];
		return CopyPattern(&m_patLines, oldHatch->ret_patline(0), m_numPatLines);
	}

	return true;
}

bool db_HatchChange::cacheSeedChange(db_hatch *oldHatch, db_hatch *newHatch)
{
	oldHatch->get_98(&m_numSeeds);

	//reallocate
	if (m_seeds)
	{
		delete [] m_seeds;
		m_seeds = NULL;
	}

	if (m_numSeeds > 0)
	{
		m_seeds = new sds_point[m_numSeeds];
		
		for (int i = 0; i < m_numSeeds; i++)
			oldHatch->get_seed(m_seeds[i], i);
	}

	return true;
}

// space must be allocated prior to calling 
bool db_HatchChange::CopyPattern(db_hatchpatlinelink **toPat, db_hatchpatlinelink *fromPat, int numPatLines)
{
    if (numPatLines > 0 && fromPat != NULL) {

		if (!toPat || !*toPat)
			return false;

//        memcpy(*toPat, fromPat, numPatLines * sizeof(fromPat[0]));

		for (int i = 0; i < numPatLines; i++)
		{
			(*toPat)[i].ang = fromPat[i].ang;
			(*toPat)[i].base[0] = fromPat[i].base[0];
			(*toPat)[i].base[1] = fromPat[i].base[1];
			(*toPat)[i].offset[0] = fromPat[i].offset[0];
			(*toPat)[i].offset[1] = fromPat[i].offset[1];
			(*toPat)[i].ndashes = fromPat[i].ndashes;

            (*toPat)[i].dash=NULL;
            if ((*toPat)[i].ndashes<1 || fromPat[i].dash==NULL) 
				continue;
            (*toPat)[i].dash=new sds_real[(*toPat)[i].ndashes];
            memcpy((*toPat)[i].dash, fromPat[i].dash, (*toPat)[i].ndashes*sizeof(sds_real));
        }
    }

	return true;
}

bool db_HatchChange::SwitchChanges(db_drawing *pDrawing, db_handitem *pItem)
{
	db_hatch * hatch = (db_hatch*) pItem;

	switch (m_hatchType)
	{
	case BOUNDARY:
		{
			// store temporary data
			int tempNum;
			hatch->get_91(&tempNum);

			db_hatchpathlink *tempPaths = new db_hatchpathlink[tempNum];

			for (int i = 0; i < tempNum; i++)
			{
				tempPaths[i] = *(hatch->ret_path(i));
				tempPaths[i].numBoundObjs = hatch->ret_path(i)->numBoundObjs;

				if (hatch->ret_path(i)->numBoundObjs > 0)
				{
					db_hatchboundobjslink *oldBoundObjs;
					hatch->get_pathboundobj(i, &oldBoundObjs);

					// just storing association info - don't need to add pointers
					tempPaths[i].boundObjs = new db_hatchboundobjslink (*oldBoundObjs);
				}
			}

			// copy current data to hatch
			hatch->set_91(m_numPaths);
			
			// need to make sure assoc flag is set correctly
			bool assoc = false;

			for (i = 0; i < m_numPaths; i++)
			{
				hatch->set_path(i, &(m_paths[i]));
				hatch->set_pathBoundaryAndAssociate(i, m_paths[i].numBoundObjs, m_paths[i].boundObjs);

				if (m_paths[i].numBoundObjs > 0)
					assoc = true;

				// Set numBoundObjs & boundObjs to 0/NULL since the hatch now points to them.  We
				// don't want these objects to be destroyed.
				m_paths[i].numBoundObjs = 0;
				m_paths[i].boundObjs = NULL;
			}
			hatch->set_71(assoc);

			// copy temp data to db_HatchChange
			if (m_paths)
				delete [] m_paths;

			m_paths = new db_hatchpathlink[tempNum];

			for (i = 0; i < tempNum; i++)
			{
				m_paths[i] = tempPaths[i];
				m_paths[i].numBoundObjs = tempPaths[i].numBoundObjs;

				// don't delete m_paths boundary objects since they were just assigned to the hatch
				// hatch will take care of deleting
				m_paths[i].boundObjs = tempPaths[i].boundObjs;

				// don't want tempPaths boundary objects deleted since they were just reassigned
				tempPaths[i].boundObjs = NULL;
			}

			delete [] tempPaths;
		}
		break;

	case PATTERN:
		{
			// store current hatch data in temporary area
			int tempNum;
			hatch->get_78(&tempNum);

			db_hatchpatlinelink *tempPat = new db_hatchpatlinelink[tempNum];
			if (!CopyPattern(&tempPat, hatch->ret_patline(0), tempNum))
				return false;

			// copy current data to hatch
			hatch->set_78(m_numPatLines); // set 78 reallocates memory
			db_hatchpatlinelink  *hatchlines = hatch->ret_patline(0);
			if (!CopyPattern(&hatchlines, m_patLines, m_numPatLines))
				return false;

			// copy temp data to member vars
			m_numPatLines = tempNum;

			// need to realloc m_patLines
			if (m_patLines)
			{
				delete [] m_patLines;
				m_patLines = NULL;
			}
			if (m_numPatLines > 0)
			{
				m_patLines = new db_hatchpatlinelink[m_numPatLines];
				if (!CopyPattern(&m_patLines, tempPat, m_numPatLines))
					return false;
			}

			delete [] tempPat;
		}
		break;

	case SEED:
		{
			// store current hatch data in temp area
			int tempNum;
			hatch->get_98(&tempNum);

			sds_point *tempSeeds = new sds_point[tempNum];
			for (int i = 0; i < tempNum; i++)
				hatch->get_seed(tempSeeds[i], i);

			// copy current data to hatch
			hatch->set_10(m_seeds, m_numSeeds);

			// copy temp data to member vars
			m_numSeeds = tempNum;

			if (m_seeds)
			{ 
				delete [] m_seeds;
				m_seeds = NULL;
			}
			if (m_numSeeds > 0)
			{
				m_seeds = new sds_point[m_numSeeds];
				for (i = 0; i < m_numSeeds; i++)
				{
					m_seeds[i][0] = tempSeeds[i][0];
					m_seeds[i][1] = tempSeeds[i][1];
				}
			}
			delete[] tempSeeds;
		}
		break;

	default:
		ASSERT(false); //not implemented
	}

	return true;
}

//
// end of db_HatchChange Implementation
// *************************************************************************
