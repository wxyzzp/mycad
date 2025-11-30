// INPUTEVENT.H
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the input event class
// 
#ifndef _INPUTEVENT_H
#define _INPUTEVENT_H


class InputEvent
	{

	// TODO TODO -- HACK friend because old code was so messy
	//
	friend int SDS_SendMessage(int msg, WPARAM nFlags,LPARAM lParam);
	friend int SDS_ProcessInputEvent( InputEvent *event );

	// -----------------
	// CTORS and DTORS
	//
	public:
		InputEvent( void )
			{
			::memset( this, 0, sizeof( *this ) );
			}

		InputEvent( int type, char key )
			{
			::memset( this, 0, sizeof( *this ) );
			m_type = type;
			m_key = key;
			};

		InputEvent( int type, sds_point pt )
			{
			::memset( this, 0, sizeof( *this ) );
			m_type = type;
			ic_ptcpy( m_pt, pt );
			};

		InputEvent( int type, char *menustring )
			{
			::memset( this, 0, sizeof( *this ) );
			m_type = type;
			m_menustr = menustring;
			};

		InputEvent( const InputEvent &theOther )
			{
				m_type = theOther.m_type;
				m_key = theOther.m_key;
				m_rqcode = theOther.m_rqcode;
				m_menustr = theOther.m_menustr;
				ic_ptcpy( m_pt, theOther.m_pt );
			}

		virtual ~InputEvent( void )
			{
			m_type = 0;
			};

	// -----------------
	// OPERATIONS
	//
	public:
		int			GetType( void )
						{
						return m_type;
						}

		char		GetKey( void )
						{
						return m_key;
						};

		void		GetPoint( sds_point pt )
						{
						ic_ptcpy( pt, m_pt );
						};

		const char* GetMenuString( void )
						{
						return m_menustr;
						};

		int			GetRQCode( void )
						{
						return m_rqcode;
						}

		bool IsValid( void )
			{
			return CHECKSTRUCTPTR( this );
			}

	// -----------------
	// internal data
	private:
		int			m_type;
		char		m_key;
		sds_point	m_pt;
		char*		m_menustr;

		// special and not need forever
		int			m_rqcode;
	};







#endif // _INPUTEVENT_H

