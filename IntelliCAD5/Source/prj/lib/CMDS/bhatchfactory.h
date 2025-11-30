/* F:\ICADDEV\PRJ\LIB\CMDS\BHATCHFACTORY.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _BHATCHFACTORY_H
#define _BHATCHFACTORY_H

class CBHatchBuilder;

class BHatchBuilderFactory
{
public:
	virtual CBHatchBuilder *CreateHatchBuilder(sds_point extrusion);
};

#endif

