// File: UndoSaver.h
//
// CUndoSaver class definition
//

#pragma once


#include "db.h"


//A class for saving changes of a db_handitem to the undo stack.
// Author: Dmitry Gavrilov.
// Steps of using:
// - create a CUndoSaver object with your non-modified db_handitem object,
// - modify it using direct methods (like set_10), but NOT with entmod (it cares about undo itself),
// - call CUndoSaver::saveToUndo,
// - delete the CUndoSaver object.

class CUndoSaver
{
public:
	CUndoSaver(db_handitem* pItem, db_drawing* pDrw);
	~CUndoSaver();
	void saveToUndo();

protected:
	db_handitem		*m_pProcessedItem, *m_pOriginalItem;
	db_drawing*		m_pDrw;
};
