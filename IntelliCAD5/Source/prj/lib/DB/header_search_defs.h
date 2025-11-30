/* C:\ICAD\PRJ\LIB\DB\HEADER_SEARCH_DEFS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "header.h"

#ifndef INC_HEADER_SEARCH_DEFS_H
#define INC_HEADER_SEARCH_DEFS_H
struct headershortsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,short);
	short (db_header::*retvarfn)(db_drawing *);
	bool hasmin;
	bool hasmax;
	bool readonly;
	short min;
	short max;
	};

class headershortsearch
	{
public:
	headershortsearch::headershortsearch(void);
	headershortsearch::~headershortsearch(void);
	struct headershortsearchstru *FindRec(char *name);
private:
	struct headershortsearchstru *data;
	short numentries;
	};

struct headershtstrsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,short);
	short (db_header::*retvarfn)(db_drawing *);
	bool hasmin;
	bool hasmax;
	bool readonly;
	short min;
	short max;
	};

class headershtstrsearch
	{
public:
	headershtstrsearch::headershtstrsearch(void);
	headershtstrsearch::~headershtstrsearch(void);
	struct headershtstrsearchstru *FindRec(char *name);
private:
	struct headershtstrsearchstru *data;
	short numentries;
	};

struct headerrealsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,sds_real);
	sds_real (db_header::*retvarfn)(db_drawing *);
	bool hasmin;
	bool hasmax;
	bool readonly;
	sds_real min;
	sds_real max;
	};

class headerrealsearch
	{
public:
	headerrealsearch::headerrealsearch(void);
	headerrealsearch::~headerrealsearch(void);
	struct headerrealsearchstru *FindRec(char *name);
private:
	struct headerrealsearchstru *data;
	short numentries;
	};

struct headerpointsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,sds_point);
	void  (db_header::*getvarfn)(db_drawing *,sds_point);
	bool readonly;
	};

class headerpointsearch
	{
public:
	headerpointsearch::headerpointsearch(void);
	headerpointsearch::~headerpointsearch(void);
	struct headerpointsearchstru *FindRec(char *name);
private:
	struct headerpointsearchstru *data;
	short numentries;
	};

struct header3dpointsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,sds_point);
	void  (db_header::*getvarfn)(db_drawing *,sds_point);
	bool readonly;
	};

class header3dpointsearch
	{
public:
	header3dpointsearch::header3dpointsearch(void);
	header3dpointsearch::~header3dpointsearch(void);
	struct header3dpointsearchstru *FindRec(char *name);
private:
	struct header3dpointsearchstru *data;
	short numentries;
	};

struct headerstringsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,char *);
	char * (db_header::*retvarfn)(db_drawing *);
	bool readonly;
	};

class headerstringsearch
	{
public:
	headerstringsearch::headerstringsearch(void);
	headerstringsearch::~headerstringsearch(void);
	struct headerstringsearchstru *FindRec(char *name);
private:
	struct headerstringsearchstru *data;
	short numentries;
	};

struct headerhandlesearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,db_objhandle);
	db_objhandle (db_header::*retvarfn)(db_drawing *);
	bool readonly;
	};

class headerhandlesearch
	{
public:
	headerhandlesearch::headerhandlesearch(void);
	headerhandlesearch::~headerhandlesearch(void);
	struct headerhandlesearchstru *FindRec(char *name);
private:
	struct headerhandlesearchstru *data;
	short numentries;
	};

struct headerhipsearchstru
	{
	char *name;
	short (db_header::*setvarfn)(db_drawing *,db_handitem *);
	db_handitem *(db_header::*retvarfn)(db_drawing *);
	bool readonly;
	};

class headerhipsearch
	{
public:
	headerhipsearch::headerhipsearch(void);
	headerhipsearch::~headerhipsearch(void);
	struct headerhipsearchstru *FindRec(char *name);
private:
	struct headerhipsearchstru *data;
	short numentries;
	};

#endif
