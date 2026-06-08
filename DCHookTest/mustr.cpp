#include <windows.h>
#pragma hdrstop
#include "mustr.h"

__mustr::__mustr( const char *str )
{
	constructor(str, str ? strlen(str) : 0);
}
__mustr::__mustr( const wchar_t *str )
{
	constructor(str, str ? wcslen(str) : 0);
}
void __mustr::constructor(const char *str, int len)
{
	buffer = (void*)str;
	length = len;
	type = 1;
	newbuffer = NULL;
}
void __mustr::constructor(const wchar_t *str, int len)
{
	buffer = (void*)str;
	length = len;
	type = 2;
	newbuffer = NULL;
}
__mustr::~__mustr()
{
	delete[] newbuffer;
}
__mustr::operator char *()
{
	if ( type == 1 ) return (char*)buffer;
	if ( newbuffer ) return (char*)newbuffer;

	// Unicode to Ansi

	newbuffer = new char[ length*2 + 1 ];
	int size = WideCharToMultiByte( CP_ACP, 0, (wchar_t*)buffer, length, newbuffer, length*2, NULL, NULL );
	newbuffer[size] = '\0';
	return (char*)newbuffer;
}
__mustr::operator wchar_t *()
{
	if ( type == 2 ) return (wchar_t*)buffer;
	if ( newbuffer ) return (wchar_t*)newbuffer;

	// Ansi to Unicode

	*(wchar_t**)&newbuffer = new wchar_t[ length*2 + 1 ];
	int size = MultiByteToWideChar( CP_ACP, 0, (char*)buffer, length, (wchar_t*)newbuffer, length*2 );
	((wchar_t*)newbuffer)[size] = '\0';
	return (wchar_t*)newbuffer;
}
