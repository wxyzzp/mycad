#pragma once
#ifndef _DBEXPORT_H_
#define _DBEXPORT_H_

#if defined( DBDLL )
	#define	DB_CLASS __declspec( dllexport)
	#define	DB_DECLSPEC __declspec( dllexport)
#else 
	#if defined( DBSTATIC )
		#define	DB_CLASS 
		#define	DB_DECLSPEC 
	#else
		#define	DB_CLASS __declspec( dllimport)
		#define	DB_DECLSPEC __declspec( dllimport)
	#endif
#endif

#ifndef	DB_API
	#define	DB_API extern "C" DB_CLASS
#endif

#endif
