/* source/prj/icad/LICENSEDCOMPONENTS.H
 * Copyright (C) 2001 IntelliCAD Technology Consortium. All rights reserved.
 *
 * Abstract
 * CLicensedComponents class : declaration
 *
 */

#ifndef __LICENSEDCOMPONENTS_H__
#define __LICENSEDCOMPONENTS_H__

const int ComponentsCount = 5;

class CLicensedComponents
{

	public:
		enum Components
		{
			VBA = 0,
			RASTER,
			RENDER,
			SPELLCHECK,
			QRENDER
		};

		CLicensedComponents();

		bool IsAccessible(Components aComponent)
		{
			return m_bIsAccessible[aComponent];
		}

		void SetAccessibility(Components aComponent, bool NewValue)
		{
			m_bIsAccessible[aComponent] = NewValue;
			return;
		}

		bool IsCommandAccessible(const char*);

	private:
		Components CLicensedComponents::CommandNumber2ComponentNumber(int CommandNumber);

		bool			m_bIsAccessible[ComponentsCount];
		static char*	Commands[];

};

#endif  __LICENSEDCOMPONENTS_H__
