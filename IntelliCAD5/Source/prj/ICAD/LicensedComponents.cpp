/* source/prj/icad/LICENSEDCOMPONENTS.CPP
 * Copyright (C) 2000 IntelliCAD Technology Consortium. All rights reserved.
 *
 * Abstract
 * CLicensedComponents class : implementation
 *
 */

#include "stdafx.h"
#include "LicensedComponents.h"

const int RenderCommandsCount		= 6;
const int RasterCommandsCount		= 8;
const int VBACommandsCount			= 4;
const int SpellCheckerCommandsCount	= 1;
const int QuickRenderCommandsCount	= 2;

const int CommandsCount =	RenderCommandsCount			+
							RasterCommandsCount			+
							VBACommandsCount			+
							SpellCheckerCommandsCount	+
							QuickRenderCommandsCount;

char*
CLicensedComponents::Commands[CommandsCount] = {/*DNT*/
												// DON'T change the order of components
												// (see CommandNumber2ComponentNumber method)
												
												// Render commands
												"RENDER",
												"FULLRENDER",
												"MATERIALS",
												"BACKGROUNDS",
												"LIGHTING",
												"SETRENDER",
												// Raster commands
												"IEIMAGE",
												"IEMANAGE",
												"IEADJUST",
												"IETRANSPARENCY",
												"IEQUALITY",
												"IEFRAME",
												"DRAWORDER",
												"IEHELP",
												// VBA commands
												"VBA",
												"VBARUN",
												"VBALOAD",
												"SECURITY",
												// SpellChecker commands
												"SPELL",
												// QuickRender commands
												"QRENDER",
												"QRENDERSET"
												};

CLicensedComponents::CLicensedComponents()
{
	for(int i = 0; i < ComponentsCount; i++)
		m_bIsAccessible[i] = true;
}

CLicensedComponents::Components
CLicensedComponents::CommandNumber2ComponentNumber(int CommandNumber)
{
	ASSERT(CommandNumber >=0 && CommandNumber < CommandsCount);

	if(CommandNumber < RenderCommandsCount)
		return RENDER;
	else
		if(CommandNumber < RenderCommandsCount + RasterCommandsCount)
			return RASTER;
		else
			if(CommandNumber < RenderCommandsCount + RasterCommandsCount + VBACommandsCount)
				return VBA;
			else
				if(CommandNumber < RenderCommandsCount + RasterCommandsCount + VBACommandsCount + SpellCheckerCommandsCount)
					return SPELLCHECK;
				else
					return QRENDER;
}

bool
CLicensedComponents::IsCommandAccessible(const char* aCommand)
{
	CString str(aCommand);

	str.MakeUpper();

	for(int i = CommandsCount - 1; i >= 0; i--)		// in reverse order because of the "RENDER" and other "*RENDER*" strings
		if(str.Find(Commands[i], 0) != -1)
			return IsAccessible(CommandNumber2ComponentNumber(i));

	return true;
}
