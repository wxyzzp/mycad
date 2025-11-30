inline 
void exctrans(unsigned int code, _EXCEPTION_POINTERS*)
{
    throw CEhSException(code);
};

/*-------------------------------------------------------------------------*//**
Structired exception translator. You should not invoke this function directly.
This function invoked by run-time system when structured exception raised. It
throws C++ exception of class sexception.
@param code =>	code of Win32 exception
@param second	=> unknown
@author	Alexey Malov
*//*--------------------------------------------------------------------------*/
void sexception_translator(unsigned int code, _EXCEPTION_POINTERS*);

inline 
exctrans_installer::exctrans_installer()
{
    m_saved_exctrans = _set_se_translator(&sexception_translator);
};

inline 
sexception_translator_installer::~sexception_translator_installer()
{
    _set_se_translator(m_saved_exctrans);
};

inline 
int sexception::code() const
{
    return m_code;
};

inline 
CEhSException::CEhSException(int code):
m_code( code ),
m_str( make_str( code ) )
{
	// Nothing to do.
}; // function sexception_t::sexception_t
