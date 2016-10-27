#ifndef __MUSTR_H
#define	__MUSTR_H

// ”Ä—p UNICODE <-> ANSI conversion class

class __mustr {
protected:
	void *buffer;
	char *newbuffer;
	int length;
	int type;
public:
	__mustr( const char *str );
	__mustr( const char *str, int len )
		{ constructor(str, len); }
	__mustr( const wchar_t *str );
protected:
	void constructor(const char *str, int len);
	void constructor(const wchar_t *str, int len);
public:
	~__mustr();
	operator char *();
	operator unsigned char *()
		{ return (unsigned char*)(char*)(*this); }
	operator wchar_t *();
	char *str()
		{ return (char*)(*this); }
	wchar_t *wstr()
		{ return (wchar_t*)(*this); }
};

#endif	// __MUSTR_H
