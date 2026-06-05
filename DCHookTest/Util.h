//---------------------------------------------------------------------------
#ifndef UtilH
#define UtilH
//---------------------------------------------------------------------------

/*------------------------------------------*/
/*		Configuration						*/
/*------------------------------------------*/
#define	USE_DDE		1	// Popup SearchÇÕÅADDEÇ≈çsÇ§,
						// otherwise uses WM_COPYDATA message. 

#ifdef USE_UNICODE
#define	DDE_SERVICE_NAME	"PDICU"
//#define	REG_PDICEXE_OLD		"Software\\ReliefOffice\\PDICU"
#define	REG_PDICEXE			"Software\\ReliefOffice\\PDIC2"
#define	APPREGNAME			"DokoPopU"
#define	NAME_PDICEXE		"PDICU.EXE"
#define	NAME_DKPPTXT		"DKPU.TXT"
#define	NAME_DKPPHKDLL		"DKPUHK.DLL"
#define	NAME_BMPFILE		"DKPU.BMP"
#define	FMTS				"%ws"
#else
#define	DDE_SERVICE_NAME	"PDICW"
#define	REG_PDICEXE			"Software\\ReliefOffice\\PDICW32"
#define	APPREGNAME			"DokoPop"
#define	NAME_PDICEXE		"PDICW32.EXE"
#define	NAME_DKPPTXT		"DKPP.TXT"
#define	NAME_DKPPHKDLL		"DKPPHK.DLL"
#define	NAME_BMPFILE		"DKPP.BMP"
#define	FMTS				"%s"
#endif
extern const char APPNAME[];

/*------------------------------------------*/
/*		Type Definitions					*/
/*------------------------------------------*/

#define	WMCD_EXISTCHECK		0x4000

#define	WM_GET_VERSION		(WM_APP+0x300)

#define	foreach(obj, it, type) \
	for (type::iterator it=(obj).begin();it!=(obj).end();it++)

#ifdef __VCL0_H__

#define	VCL_SIZEABLE()	do {BorderStyle = bsSizeable;} while(0)	//TODO: VCL Bug

class TMyIni {
public:
	HKEY hroot;
public:
	TMyIni( HKEY root, const char *soft, bool can_create );
	~TMyIni();
	void WriteInteger( const char *section, const char *key, int val );
	int ReadInteger( const char *section, const char *key, int val );
	void WriteString( const char *section, const char *key, const char *str );
	AnsiString ReadString( const char *section, const char *key, const char *str );
	void ReadValues( const char *section, TStrings *strs );
};

class TAppIni : public TMyIni {
typedef TMyIni super;
public:
	TAppIni();
};

void LoadForm( const char *section, TForm *form, HWND hwnd );
void SaveForm( const char *section, TForm *form, HWND hwnd );
#endif

/*------------------------------------------*/
/*		Prototypes							*/
/*------------------------------------------*/
bool LaunchPdic();
bool MODIInstalled();
bool AMODIRunable();
bool LaunchAMODI();
void TerminateAMODI();
void ShowAMODI();

bool CheckPassword( const char *str );
void CheckWOW64();
void MoveToTop( HWND hwnd );
void MoveCenter( HWND hwnd, HWND hwndParent );
void GetScreenSize(HWND hwndBase, RECT *rcWork, RECT *rcScreen=NULL);
bool _WinExec( const char *cmd, int show, int waittime, const char *dir=NULL);
HANDLE WinExecEx( const char *cmd, int show, const char *dir=NULL, const char *title=NULL );
HWND FindApp(const char *clsname, const char *wndname, const char *appname);
HWND FindPrev();
const char *GetStrVersion();
unsigned GetVersionValue();
int CheckVersion(HWND hwnd);
HWND FindAMODI();
HWND FindPopupWindow( );

void ShowManual( HWND hwnd );
void ShowLatestVersion();

unsigned GetDNFVersion();	// get .NET Framework version

void dbw( const char *format, ... );

#ifdef _DEBUG
#define	DBW	dbw
#else
#define	DBW	(void)
#endif

/*------------------------------------------*/
/*		Global Variables					*/
/*------------------------------------------*/
extern bool WindowsNT;
extern bool fWow64;

#endif

