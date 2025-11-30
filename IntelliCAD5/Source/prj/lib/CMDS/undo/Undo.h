#include "UndoTLB.h"

#define	IDR_UNDOMGR			1
#define	IDR_PARENTUNDOUNIT	1


#if	defined(ASSERT) && !defined(UNDO_KEEPASSERT)
#undef	ASSERT
#endif

