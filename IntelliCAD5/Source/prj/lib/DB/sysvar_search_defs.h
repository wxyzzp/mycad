/* C:\ICAD\PRJ\LIB\DB\SYSVAR_SEARCH_DEFS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef INC_SYSVAR_SEARCH_DEFS_H
#define INC_SYSVAR_SEARCH_DEFS_H
struct sysvarshortsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(short);
	short (db_sysvar::*retvarfn)(void);
	bool hasmin;
	bool hasmax;
	bool readonly;
	short min;
	short max;
	bool isconfig;
	};

class sysvarshortsearch
	{
public:
	sysvarshortsearch::sysvarshortsearch(void);
	sysvarshortsearch::~sysvarshortsearch(void);
	struct sysvarshortsearchstru *FindRec(char *name);
private:
	struct sysvarshortsearchstru *data;
	short numentries;
	};

struct sysvarshtstrsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(short);
	short (db_sysvar::*retvarfn)(void);
	bool hasmin;
	bool hasmax;
	bool readonly;
	short min;
	short max;
	bool isconfig;
	};

class sysvarshtstrsearch
	{
public:
	sysvarshtstrsearch::sysvarshtstrsearch(void);
	sysvarshtstrsearch::~sysvarshtstrsearch(void);
	struct sysvarshtstrsearchstru *FindRec(char *name);
private:
	struct sysvarshtstrsearchstru *data;
	short numentries;
	};

struct sysvarrealsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(sds_real);
	sds_real (db_sysvar::*retvarfn)(void);
	bool hasmin;
	bool hasmax;
	bool readonly;
	sds_real min;
	sds_real max;
	bool isconfig;
	};

class sysvarrealsearch
	{
public:
	sysvarrealsearch::sysvarrealsearch(void);
	sysvarrealsearch::~sysvarrealsearch(void);
	struct sysvarrealsearchstru *FindRec(char *name);
private:
	struct sysvarrealsearchstru *data;
	short numentries;
	};

struct sysvarpointsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(sds_point);
	void  (db_sysvar::*getvarfn)(sds_point);
	bool readonly;
	bool isconfig;
	};

class sysvarpointsearch
	{
public:
	sysvarpointsearch::sysvarpointsearch(void);
	sysvarpointsearch::~sysvarpointsearch(void);
	struct sysvarpointsearchstru *FindRec(char *name);
private:
	struct sysvarpointsearchstru *data;
	short numentries;
	};

struct sysvar3dpointsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(sds_point);
	void  (db_sysvar::*getvarfn)(sds_point);
	bool readonly;
	bool isconfig;
	};

class sysvar3dpointsearch
	{
public:
	sysvar3dpointsearch::sysvar3dpointsearch(void);
	sysvar3dpointsearch::~sysvar3dpointsearch(void);
	struct sysvar3dpointsearchstru *FindRec(char *name);
private:
	struct sysvar3dpointsearchstru *data;
	short numentries;
	};

struct sysvarstringsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(char *);
	char * (db_sysvar::*retvarfn)(void);
	bool readonly;
	bool isconfig;
	};

class sysvarstringsearch
	{
public:
	sysvarstringsearch::sysvarstringsearch(void);
	sysvarstringsearch::~sysvarstringsearch(void);
	struct sysvarstringsearchstru *FindRec(char *name);
private:
	struct sysvarstringsearchstru *data;
	short numentries;
	};

struct sysvarhandlesearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(db_objhandle);
	db_objhandle (db_sysvar::*retvarfn)(void);
	bool readonly;
	bool isconfig;
	};

class sysvarhandlesearch
	{
public:
	sysvarhandlesearch::sysvarhandlesearch(void);
	sysvarhandlesearch::~sysvarhandlesearch(void);
	struct sysvarhandlesearchstru *FindRec(char *name);
private:
	struct sysvarhandlesearchstru *data;
	short numentries;
	};

struct sysvarhipsearchstru
	{
	char *name;
	short (db_sysvar::*setvarfn)(db_handitem *);
	db_handitem *(db_sysvar::*retvarfn)(void);
	bool readonly;
	bool isconfig;
	};

class sysvarhipsearch
	{
public:
	sysvarhipsearch::sysvarhipsearch(void);
	sysvarhipsearch::~sysvarhipsearch(void);
	struct sysvarhipsearchstru *FindRec(char *name);
private:
	struct sysvarhipsearchstru *data;
	short numentries;
	};

#endif
