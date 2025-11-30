// File  :
// Author: Alexey Malov
#include "stdafx.h"
#include "AsSatStream.h"
#include <iomanip>
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif

 /*----------------------------------------------------------------------*/
void CAsSatStream::eatWhiteSpace()
{
    char c(m_stream.peek());
    while (c && (' ' == c || '\t' == c || '\r' == c || '\n' == c))
    {
        m_stream.read(&c, 1);
        c = m_stream.peek();
    }
}

void CAsSatStream::readField(char *pField)
{
    eatWhiteSpace();
    int i = 0;
    char c(m_stream.peek());
    while (c && ' ' != c)
    {
        m_stream.read(&c, 1);
        pField[i] = c;
        i++;
        c = m_stream.peek();
    }
    pField[i] = 0;
}

size_t CAsSatStream::read(void *pBuffer, size_t length)
{
    m_stream.read((char*)pBuffer, length);
    return length;
}

logical CAsSatStream::read_logical(const char *f /* ="F" */, const char *t /* ="T" */)
{
    eatWhiteSpace();
    char c(m_stream.peek());
    if ((c >= '0' && c <= '9') || '-' == c)
        return (read_long() ? TRUE : FALSE);

    char s[50];
    int l = 0;
    while (EOF != c && ' ' != c)
    {
        m_stream.read(&c, 1);
        s[l++] = c;
        c = m_stream.peek();
    }
    s[l] = 0;

    if (0 == strncmp(s, f, l))
        return FALSE;

    if (0 == strncmp(s, t, l))
        return TRUE;

    sys_error(1);
    return FALSE;
}

int CAsSatStream::read_enum(enum_table const &eTable)
{
    eatWhiteSpace();
    char c(m_stream.peek());
    if ((c >= '0' && c <= '9') || '-' == c)
        return (int) read_long();

    char s[50];
    char *p = s;
    while (EOF != c && ' ' != c)
    {
        m_stream.read(&c, 1);
        *p++ = c;
        c = m_stream.peek();
    }
    *p = 0;

    return eTable.value(s);
}

char CAsSatStream::read_char()
{
    char c;
    m_stream.read(&c, 1);
    return c;
}

short CAsSatStream::read_short()
{
    char buf[200];
    readField(buf);
    return atol(buf);
}

long CAsSatStream::read_long()
{
    char buf[200];
    readField(buf);
    return atol(buf);
}

float CAsSatStream::read_float()
{
    char buf[200];
    readField(buf);
    return atof(buf);
}

double CAsSatStream::read_double()
{
    char buf[200];
    readField(buf);
    return atof(buf);
}

char *CAsSatStream::read_string(int &len)
{
    len = read_long();
    char c; m_stream.read(&c, 1); assert(' ' == c); // Skip single space
    char *pBuffer = ACIS_NEW char [len + 1];

    read(pBuffer, len);
    pBuffer[len] = 0;
    
    return pBuffer;
}

size_t CAsSatStream::read_string(char *pBuffer, size_t maxlen /* = 0 */)
{
    size_t len = read_long();
    char c; m_stream.read(&c, 1); assert(' ' == c); // Skip single space
    if (maxlen > 0 && len > maxlen)
        sys_error(1);

    read(pBuffer, len);
    pBuffer[len] = 0;

    return len;
}

void *CAsSatStream::read_pointer()
{
    eatWhiteSpace();
    char c;
    m_stream.read(&c, 1);
    while ('$' != c)
        m_stream.read(&c, 1);

    return (void *) read_long();
}

logical CAsSatStream::read_header(int &version, int &totalCount, int &givenCount, int &extra)
{
    FilePosition pos = set_mark();

    logical bIsOk = TRUE;

    version    = read_long();
    totalCount = read_long();
    givenCount = read_long();
    extra      = read_long();

    if (bIsOk)
    {
        eatWhiteSpace();
        if (EOF == m_stream.peek())
            bIsOk = FALSE;
    }

    if (!bIsOk)
        goto_mark(pos);

    return bIsOk;
}

int CAsSatStream::read_id(char *pID, int bufLen /* = 0 */)
{
    eatWhiteSpace();
    char c(m_stream.peek());
    int len = 0;
    while (' ' != c && EOF != c)
    {
        m_stream.read(&c, 1);
        if (bufLen > 0 && len < bufLen)
            pID[len++] = c;

        c = m_stream.peek();
    }

    if (EOF == c && 0 == len)
        sys_error(1);
    else if (bufLen > 0 && len >= bufLen)
    {
        sys_error(1);
        len = bufLen - 1;
    }
    pID[len] = 0;

    return len;
}

int CAsSatStream::read_sequence()
{
    eatWhiteSpace();
    char c(m_stream.peek());
    if ('-' == c)
    {
        m_stream.read(&c, 1);
        return read_long();
    }

    return -1;
}

logical CAsSatStream::read_subtype_start()
{
    eatWhiteSpace();
    char c(m_stream.peek());
    if ('{' == c)
    {
        m_stream.read(&c, 1);
        return TRUE;
    }

    return FALSE;
}

logical CAsSatStream::read_subtype_end()
{
    eatWhiteSpace();
    char c(m_stream.peek());
    if ('}' == c)
    {
        m_stream.read(&c, 1);
        return TRUE;
    }

    return FALSE;
}

TaggedData* CAsSatStream::read_data()
{
    eatWhiteSpace();
    char c(m_stream.peek());

    if ('$' == c)
    {
        m_stream.read(&c, 1); // Reads the '$'
        return ACIS_NEW TaggedData(TaggedData::pointer_type, (void *) read_long());
    }
    if ('{' == c)
    {
        m_stream.read(&c, 1); // Reads the '{'
        return ACIS_NEW TaggedData(TaggedData::subtype_start, NULL);
    }
    if ('}' == c)
    {
        m_stream.read(&c, 1); // Reads the '}'
        return ACIS_NEW TaggedData(TaggedData::subtype_end, NULL);
    }
    if ('#' == c)
    {
        m_stream.read(&c, 1); // Reads the '#'
        c = m_stream.peek();
        while ('\n' != c && '\r' != c)
        {
            m_stream.read(&c, 1);
            if (EOF == c)
            {
                sys_error(1);
                break;
            }
            c = m_stream.peek();
        }

        eatWhiteSpace();
        return NULL;
    }

    char *pCurrentString = NULL;
    int strLen = 0, strCount = 0;
    while (EOF != c && '$' != c && '#' != c && '{' != c && '}' != c)
    {
        m_stream.read(&c, 1); // Reads the last peeked character

        if (strCount >= strLen)
        {
            strLen += 60;
            char *pNewString = ACIS_NEW char [strLen + 1];
            if (NULL != pCurrentString)
            {
                strncpy(pNewString, pCurrentString, strCount);
                delete pCurrentString;
            }

            pCurrentString = pNewString;
        }

        pCurrentString[strCount++] = c;
        c = m_stream.peek();
    }

    if (NULL != pCurrentString)
        pCurrentString[strCount] = 0;

    return ACIS_NEW TaggedData(TaggedData::literal_char_string, pCurrentString);
}

SPAvector CAsSatStream::read_vector()
{
    double x = read_double();
    double y = read_double();
    double z = read_double();

    return SPAvector(x, y, z);
}

SPAposition CAsSatStream::read_position()
{
    double x = read_double();
    double y = read_double();
    double z = read_double();

    return SPAposition(x, y, z);
}

FilePosition CAsSatStream::set_mark()
{
    return m_stream.tellg();
}

FilePosition CAsSatStream::goto_mark(FilePosition filePos)
{
    m_stream.seekg(filePos);
    return 0;
}

void CAsSatStream::write_logical
(
logical b,
const char *f, 
const char *t
)
{
    if (b)
        m_stream << t << ' ';
    else
        m_stream << f << ' ';
}

void CAsSatStream::write_enum
(
int n, 
enum_table const& tbl
)
{
    write_literal_string(tbl.string(n));
}

void CAsSatStream::write_char(char c)
{
    m_stream << c;
}

void CAsSatStream::write_short(short i)
{
    m_stream << i << ' ';
}

void CAsSatStream::write_long(long l)
{
    m_stream << l << ' ';
}

void CAsSatStream::write_float(float f)
{
    m_stream << std::setprecision(FLT_DIG) << f << ' ';
}

void CAsSatStream::write_double(double d)
{
    m_stream << std::setprecision(DBL_DIG) << d << ' ';
}

void CAsSatStream::write_string
(
const char* str, 
size_t len
)
{
    len = strlen(str);
    write_long(len);
    m_stream.write(str, len);
    write_newline();
}

void CAsSatStream::write_literal_string
(
const char* str, 
size_t len
)
{
    len = strlen(str);
    m_stream.write(str, len);
    m_stream << ' ';
}

void CAsSatStream::write_pointer(void *p)
{
    m_stream << '$' << (long)p << ' ';
}

void CAsSatStream::write_header
(
int version, 
int totalCount, 
int givenCount, 
int extra
)
{
    write_long(version);
    write_long(totalCount);
    write_long(givenCount);
    write_long(extra);
    write_newline();
}

void CAsSatStream::write_id
(
const char* sId, 
int n
)
{
    int l = strlen(sId);
    m_stream.write(sId, l);
    if (n > 1)
        m_stream << '-';
    else
        m_stream << ' ';
}

void CAsSatStream::write_sequence(int i)
{
    m_stream << '-' << long(i) << ' ';
}

void CAsSatStream::write_newline(int n)
{
    while (n-- > 0)
    {
        m_stream << "\r\n";
    }
}

void CAsSatStream::write_terminator()
{
    m_stream << "#";
    write_newline();
}

void CAsSatStream::write_subtype_start()
{
    m_stream << "{" << ' ';
}

void CAsSatStream::write_subtype_end()
{
    m_stream << "}" << ' ';
}

void CAsSatStream::write_vector(const SPAvector& vec)
{
    write_double(vec.x());
    write_double(vec.y());
    write_double(vec.z());
}

void CAsSatStream::write_position(const SPAposition& pos)
{
    write_double(pos.x());
    write_double(pos.y());
    write_double(pos.z());
}

