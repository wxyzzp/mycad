// File  :
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASSATSTREAM_H_
#define _ASSATSTREAM_H_

#include <strstream>
using std::strstream;
using std::ios;

class CAsSatStream: public FileInterface
{
    strstream m_stream; // Stream providing the data

    void eatWhiteSpace ();
    void readField (char *pField);
    size_t read (void *pBuffer, size_t length);

public:
    CAsSatStream()
    :
    FileInterface()
    {}
    CAsSatStream(char* sName, int nSize, strstream::openmode mode = strstream::in | strstream::out)
    :
    FileInterface(),
    m_stream(sName, nSize, mode)
    {}
    virtual ~CAsSatStream(){} // Do not delete the stream pointer

    //--------------------
    // virtual methods from FileInterface
    void write_logical (logical, const char *f="F", const char *t="T");
    logical read_logical (const char *f="F", const char *t="T");

    void write_enum (int, enum_table const &);
    int read_enum (enum_table const &);

    void write_char (char);
    char read_char ();

    void write_short (short);
    short read_short ();

    void write_long (long);
    long read_long ();

    void write_float (float);
    float read_float ();

    void write_double (double);
    double read_double ();

    void write_string (const char *, size_t len = 0);
    char *read_string (int &);
    size_t read_string (char *buf, size_t maxlen = 0);

    void write_literal_string (const char *, size_t len = 0);

    // The following procedures are for writing specific kinds of data
    // which must be recognized when saving and restoring unkn\own
    // ENTITY types, so they cannot be implemented in terms of the
    // procedures for the basic types.

    void write_pointer(void *);
    void* read_pointer();

    void write_header( int, int, int, int );
    logical read_header( int &, int &, int &, int & );

    void write_id( const char*, int );
    int read_id( char *, int = 0 );

    void write_sequence(int);
    int read_sequence();

    void write_newline(int = 1);

    void write_terminator();

    void write_subtype_start();
    logical read_subtype_start();

    void write_subtype_end();
    logical read_subtype_end();

    TaggedData* read_data();

    void write_vector(const SPAvector& vec);
    SPAvector read_vector();

    void write_position(const SPAposition& pos);
    SPAposition read_position();

    //----------------------
    // Nethods for positioning withing the file
    FilePosition set_mark();
    FilePosition goto_mark(FilePosition);

    logical unknown_types_ok() {return true;}

    char* data() {return m_stream.str();}
    int size() {return m_stream.pcount();}
    void freeze(bool f = true) {m_stream.freeze(f);}
};


#endif
