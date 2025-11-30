#include "cmds.h"
#include "cmdsHatch.h"
#include "Icad.h"
#include "IcadDoc.h"
#include "listcoll.h"
#include "BHatchBuilder.h"
#include "GeoConvert.h"
#include "paths.h"

class PatternFileProcessor : public AbstractFileProcessor
{
public:
	PatternFileProcessor() : m_count(0) {m_fileList.SetSize(20,10);}
	void ProcessFile(LPCTSTR, WIN32_FIND_DATA& data);

	CString GetFile(int index) {if ((index >= m_count) || (index < 0))
									return "";
								return m_fileList[index];}

	int	GetCount() {return m_count;}

protected:
	CArray<CString, CString> m_fileList;
	int m_count;
};

void PatternFileProcessor::ProcessFile(LPCTSTR, WIN32_FIND_DATA& data)
{
	if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))	// skip directories
	{
		m_fileList.Add(data.cFileName);
		m_count++;
	}
}

bool cmd_FindPatternDef(CString patName, int *numlines, cmd_hpdefll **patternDef)
{
	if (patternDef)
		cmd_hatch_freehpdefll(patternDef);

	//Ebatech(cnbr) -[ 2002/9/20 Default pattern file is dependent on $MEASUREMENT.
	resbuf rb;
	SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	*numlines = cmd_hatch_loadpat(( rb.resval.rint == DB_ENGLISH_MEASUREINIT ) ?
		SDS_PATTERNFILE : SDS_ISOPATTERNFILE, patName, patternDef);
	//most likely the pattern name is in icad.pat so try that first  SDS_PATTERNFILE
	//*numlines = cmd_hatch_loadpat(SDS_ISOPATTERNFILE, patName, patternDef);
	//Ebatech(cnbr) ]-

	if (*numlines != RTERROR)
		return true;

	// try all files in the path
	PatternFileProcessor processor;

	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	FilePath	patternPath(mainWnd->m_paths->m_hatchPatPath);

	patternPath.FindFiles(processor, "*.pat");

	int fileCount = processor.GetCount();
	int i = 0;

	// if not in icad.pat or icadiso.pat search all files found on path
	while ((*numlines == RTERROR) && (i < fileCount))
	{
		*numlines = cmd_hatch_loadpat((LPCTSTR)processor.GetFile(i), patName, patternDef);
		i++;
	}

	return (*numlines != RTERROR);
}

bool cmd_SetHatchFromDef(int numlines, cmd_hpdefll *currentDef, db_hatch *hatch)
{
	//use copy of hatch pattern def so caller will still have pointer to beginning of def;
	cmd_hpdefll * curHpDef = currentDef;

	if (!curHpDef || !hatch)
		return false;

	hatch->set_78(numlines);

	//Adjust pattern line
	double xOffset, yOffset;
	double scale, hatchAngle;

	hatch->get_52(&hatchAngle);
	hatch->get_41(&scale);

	//set values
	int i = 0;
	cmd_hplinedef *curLine;
	int ndashes = 0;
	double curLength = 0.0;
	double patternBaseX, patternBaseY;

	while (curHpDef)
	{
		//need to count number of pattern lines first
		curLine = curHpDef->linedef;
		ndashes = 0;
		curLength = 0.0;

		while (curHpDef->linelen > curLength)
		{
			curLength += curLine->drawlength;
			ndashes++;
			curLine = curLine->next;
		}

		double angle = hatchAngle + curHpDef->angle;

		//base point only uses the hatch angle while the offset uses the angle of the hatch + the angle of the def.
		patternBaseX = ((curHpDef->startpt[0] * cos(hatchAngle)) - (curHpDef->startpt[1] * sin(hatchAngle))) * scale;
		patternBaseY = ((curHpDef->startpt[0] * sin(hatchAngle)) + (curHpDef->startpt[1] * cos(hatchAngle))) * scale;

		xOffset = ((curHpDef->xoffset * cos(angle)) - (curHpDef->yoffset * sin(angle))) * scale;
		yOffset = ((curHpDef->xoffset * sin(angle)) + (curHpDef->yoffset * cos(angle))) * scale;

		hatch->set_patline(i, angle, patternBaseX, patternBaseY, xOffset, yOffset, ndashes);

		//set dash info
		curLine = curHpDef->linedef;
		ndashes = 0;
		curLength = 0.0;

		double dashVal = 0;

		while (curHpDef->linelen > curLength)
		{
			curLength += curLine->drawlength;

			// if the pen is up set the length to negative for the hatch pattern def
			if (curLine->pendown)
				dashVal = curLine->drawlength;

			else
				dashVal = -1.0 * curLine->drawlength;

			dashVal *= scale;

			hatch->set_patlinedash(i, ndashes, dashVal);
			ndashes++;
			curLine = curLine->next;
		}

		i++;
		curHpDef = curHpDef->next;
	}

	return true;
}

bool cmd_SetPatternDef(db_hatch *hatch)
{
	cmd_hpdefll *patternDef = NULL;
	int numlines;
	char *patName;

	//Get hatch type - pattern is set differently if user defined.
	int type;
	hatch->get_76(&type);

	if (type = 0) //user-defined
	{
		numlines = 1;

		int crossHatch;
		hatch->get_77(&crossHatch);

		if (crossHatch)
			numlines++;

		if (!cmd_loadUserDefPattern(&patternDef, crossHatch))
			return false;

	}
	else
	{
		hatch->get_2(&patName);

		if (!cmd_FindPatternDef(patName, &numlines, &patternDef))
		{
			if (patternDef)
				cmd_hatch_freehpdefll(&patternDef);

			return false;
		}
	}

	bool result = cmd_SetHatchFromDef(numlines, patternDef, hatch);


	if (patternDef)
		cmd_hatch_freehpdefll(&patternDef);

	return result;
}

// input an existing hatch and the boundary collection.
// returns true if successful.
bool cmd_ApplyHatchToBoundryObjects(HanditemCollection *collection, db_hatch *hatch)
{
	RC rc;

	C3Point ptNormal;
	GetUcsZ(ptNormal);
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBHatchBuilder hatchBuilder( ptNormal, rc);
#else
	CBHatchBuilder hatchBuilder(HATCH_TOLERANCE, ptNormal, rc);
#endif
	ASSERT (SUCCEEDED(rc));

	//set hatch pattern definition
	if (!cmd_SetPatternDef(hatch))
		return false;

	hatchBuilder.set_hatch(hatch, false);

	// Set candidate list not to toggle
	hatchBuilder.SetToggle(false);

	int style;
	hatch->get_75(&style);

	// Set island detection
	switch (style)
	{
	case 0:
		hatchBuilder.SetIslandDetection(Nested_Islands);
		break;

	case 1:
		hatchBuilder.SetIslandDetection(Outer_Only);
		break;

	case 2:
		hatchBuilder.SetIslandDetection(Ignore_Islands);
		break;

	default:
		ASSERT(FALSE);
	}

	bool boundaryValid = hatchBuilder.AcceptHanditemColl(SDS_CURDWG, *collection, ptNormal);

	if (boundaryValid == false)
		return false;

	int numSeeds;
	hatch->get_98(&numSeeds);

	// if the user specified seed points use them as the click points, otherwise use the center if the
	// min-max box.
	if (numSeeds > 0)
	{
		sds_point seed;
		for (int i = 0; i < numSeeds; i++)
		{
			hatch->get_seed(seed, i);

			rc = hatchBuilder.ClickPoint(seed[0], seed[1]);

			if (rc != SUCCESS)
				return false;
		}
	}
	else
		{
		// Use the center of the min-max boxes as seed points. Hatching was really designed to work by
		// clicking points instead of selecting objects. The algorithm isn't perfect, but with the
		// current schedule it's the best we can do.
		db_handitem *curr = collection->GetNext(NULL);
		while (curr)
		{
			rc = hatchBuilder.ClickPoint(curr);
			if (rc != SUCCESS)
				return false;
			curr = collection->GetNext(curr);
		}
	}

	rc = hatchBuilder.Persist();

	//we want to give users a chance to reuse the hatch rather than destroying
	if (rc == SUCCESS)
	{

		HanditemCollection *selectedEnts = hatchBuilder.getHanditemCollection();

		if (selectedEnts)
		{
			//unhighlight selected entities
			cmd_drawHICollection(selectedEnts, IC_REDRAW_DRAW);

			db_handitem *entity = selectedEnts->GetNext(NULL);
			while (entity)
			{
				// ensure entity is not destroyed when list is destroyed since it is being
				// added to the drawing.
				selectedEnts->SetDestroyHI(entity, false);

				entity = selectedEnts->GetNext(entity);
			}
		}

		// Set the drawing modified flag.
		SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
	}

	return (rc == SUCCESS);
}
