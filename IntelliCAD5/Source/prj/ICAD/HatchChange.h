#ifndef __HatchChangeH__
#define __HatchChangeH__

#include "binchange.h"

// probably should have divided db_HatchChange into more classes...
class db_HatchChange : public db_BinChange
{
public:
	// Note that hatchtype of Parent defaults to use db_BinChange
	enum HatchChangeType {BOUNDARY, PATTERN, SEED, PARENT};

	db_HatchChange(ChangeType type, HatchChangeType hatchType);
	~db_HatchChange();

	// for hatch specific comparisons the entire hatch should be passed in as newval/oldval
	static bool CacheChangedData(struct db_BinChange **pBeginList, 
						struct db_BinChange **pCurrentNode, 
						void *oldval, 
						void *newval, 
						int len,
						HatchChangeType hatchtype); 


protected:
	virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem);
	
	static bool compareHatchBoundaries(db_hatch *oldHatch, db_hatch *newhatch);
	bool cacheHatchBoundaryChange(db_hatch *oldHatch, db_hatch *newhatch);

	bool CopyPattern(db_hatchpatlinelink **toPat, db_hatchpatlinelink *fromPat, int numPatLines);
	static bool comparePatternLines(db_hatch *oldHatch, db_hatch *newHatch);
	bool cachePatternChange(db_hatch *oldHatch, db_hatch *newhatch);

	static bool compareSeeds(db_hatch *oldHatch, db_hatch *newHatch);
	bool cacheSeedChange(db_hatch *oldHatch, db_hatch *newHatch);

protected:
	HatchChangeType m_hatchType;

private:
	int m_numPaths;
	db_hatchpathlink *m_paths; // if path change paths stored here

	int m_numPatLines;
	db_hatchpatlinelink *m_patLines;

	int m_numSeeds;
	sds_point *m_seeds;
};

#endif
