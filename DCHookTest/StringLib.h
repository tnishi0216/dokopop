//---------------------------------------------------------------------------
#ifndef StringLibH
#define StringLibH
//---------------------------------------------------------------------------

#include <mbctype.h>

// Constants //
#define	CODE_APOSTROPHE	0x9188	// Åf

// typedefs and macros //
#ifdef USE_UNICODE
typedef wchar_t tuchar;
typedef wchar_t tchar;
#define	LD_CHAR(c,p)	c = *p++;
#define	ST_CHAR(c,p)	*p++ = c;
#define	MOV_CHAR(p,q)	*p++ = *q++;
#define	IS_ENDCHAR(c)	(!(c))
#define	NEXT_CHAR(p)	CharNextW(p)
#undef _tcslen
#define	_tcslen			wcslen
#define	_t(x)			L##x
#define	mustr				__mustr
#include "mustr.h"
#else
typedef unsigned char tuchar;
typedef char tchar;
#define	LD_CHAR(c, p)	{c = (unsigned char)*(*(unsigned char**)&p)++; if(_ismbblead(c)) c = (unsigned short)(((unsigned short)c << 8) + (unsigned char)*(*(unsigned char**)&p)++);}
#define	ST_CHAR(c, p)	{if ((unsigned short)c > 0x100) *(*(unsigned char**)&p)++ = (unsigned char)(c >> 8); *(*(unsigned char**)&p)++ = (unsigned char)c;}
#define	MOV_CHAR(p, q)	{ unsigned char c = *(*(unsigned char**)&q)++ = *(*(unsigned char**)&p)++; if ( _ismbblead( c ) ) *(*(unsigned char**)&q)++ = *(*(unsigned char**)&p)++; }
#define	IS_ENDCHAR(c)	(!((unsigned char)(c)))
#define	NEXT_CHAR(p)	CharNextA(p)
#define	_tcslen			strlen
#define	_t(x)			x
#define	mustr(s)			(s)
#endif

typedef unsigned int uint;
typedef unsigned short ushort;

#define	STR_DIFF( p1, p2 )	( (uint)( (tchar*)(p1) - (tchar*)(p2) ) )

// prototypes //
int isalphanum( tuchar c );
bool mbIsWordChar( unsigned short c );
bool GetWord( const tchar *str, int pos, int &start, int &end, int &prevstart, bool fLongest, int wordcount, bool about, bool alnum, int numPrev );
bool mbGetWord( const tchar *str, int pos, int &start, int &end, bool fLongest, int wordcount );

// inlines //
inline bool IsWordChar( tuchar c )
{
#ifdef _UNICODE
	WORD ct;
	GetStringTypeW(CT_CTYPE1,&c,1,&ct);
	return (ct & (C1_ALPHA|C1_DIGIT)) || c=='-' || c==CODE_JPROLONG || c=='\'' || c=='_';
#else
	return isalphanum( c ) || c == '-' || c == '\'' || c >= 0xc0;
#endif
}
#if 0
inline bool IsNotWordChar( tuchar c )
{
#ifdef USE_UNICODE
	WORD ct;
	GetStringTypeW(CT_CTYPE1,&c,1,&ct);
	return (ct & (C1_SPACE|C1_PUNCT|C1_CNTRL|C1_BLANK)) ? true : false;
#else
	return !IsWordChar(c);
#endif
}
#endif

#endif
 