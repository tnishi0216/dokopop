#include <vcl.h>
#pragma	hdrstop
#include <winver.h>
#include	"pdver.h"

ProjectRCVersion::ProjectRCVersion(const TCHAR *appFName)
{
	DWORD   fvHandle;

	FVData = NULL;

	TCHAR *appname = _tcsdup( appFName );
	DWORD dwSize = GetFileVersionInfoSize( appname, &fvHandle);
	if (dwSize) {
		FVData  = (void *)new TCHAR[dwSize];
		if (!GetFileVersionInfo( appname, fvHandle, dwSize, FVData)) {
			// error
			if (FVData){
				delete[] FVData;
				FVData = NULL;
			}
		}
	}
	free( appname );
}


ProjectRCVersion::~ProjectRCVersion()
{
	delete[] FVData;
}

bool ProjectRCVersion::GetProductVersion(AnsiString &prodVersion)
{
	if (!FVData)
		return false;

	VS_FIXEDFILEINFO *pFixedFileInfo;
	UINT uLen;
	if(!VerQueryValue(FVData, _T("\\"), (LPVOID*)&pFixedFileInfo, &uLen)){
		return false;
	}
	prodVersion.sprintf(_T("%d.%d.%d"), 
		HIWORD(pFixedFileInfo->dwProductVersionMS),
		LOWORD(pFixedFileInfo->dwProductVersionMS),
		HIWORD(pFixedFileInfo->dwProductVersionLS)
		/*,LOWORD(pFixedFileInfo->dwProductVersionLS)*/);
	return true;
}

unsigned ProjectRCVersion::GetProductVersionValue()
{
	if (!FVData)
		return 0;	// error

	VS_FIXEDFILEINFO *pFixedFileInfo;
	UINT uLen;
	if(!VerQueryValue(FVData, _T("\\"), (LPVOID*)&pFixedFileInfo, &uLen)){
		return 0;	// error
	}
	return (HIWORD(pFixedFileInfo->dwProductVersionMS) << 24) |
		(LOWORD(pFixedFileInfo->dwProductVersionMS) << 16) |
		(HIWORD(pFixedFileInfo->dwProductVersionLS));
}

static const TCHAR *VERS_TRANSLATION = _T("\\varFileInfo\\translation");
static const TCHAR *VERS_STRINGFILEINFO = _T("\\StringFileInfo\\%08lx\\%s");
static const TCHAR *VERS_PRODUCTNAME = _T("ProductName");
//static const TCHAR *VERS_PRODUCTVERSION = _T("ProductVersion");
static const TCHAR *VERS_LEGALCOPYRIGHT = _T("LegalCopyright");
static const TCHAR *VERS_SPECIALBUILD = _T("SpecialBuild");

ProjectRCVersionString::ProjectRCVersionString(const TCHAR *appFName)
	:super(appFName)
{
	UINT vSize;

	if (FVData){
		// If not error in super class.
		TCHAR    subBlockName[255];
		_tcscpy(subBlockName, VERS_TRANSLATION);
		if (!VerQueryValue(FVData, subBlockName, (void **)&TransBlock, &vSize)) {
			delete[] FVData;
			FVData = NULL;
		} else {
			// 上下ワードをスワップして，wsprintf が lang-charset を正しい書式で出力するようにする
			*(DWORD *)TransBlock = MAKELONG(HIWORD(*(DWORD *)TransBlock), LOWORD(*(DWORD *)TransBlock));
		}
	}
}

bool ProjectRCVersionString::GetProductName(AnsiString &prodName)
{
	if (!FVData)
		return false;

	UINT    vSize;
	TCHAR    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_PRODUCTNAME);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void **)&pptr, &vSize)){
		return false;
	}
	prodName = pptr;
	return true;
}

#if 0
bool ProjectRCVersionString::GetProductVersion(AnsiString &prodVersion)
{
	if (!FVData)
		return false;

	UINT    vSize;
	TCHAR    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_PRODUCTVERSION);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void **)&pptr, &vSize)){
		return false;
	}
	prodVersion = pptr;
	return true;
}
#endif

bool ProjectRCVersionString::GetCopyright(AnsiString &copyright)
{
	if (!FVData)
		return false;

	UINT    vSize;
	TCHAR    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_LEGALCOPYRIGHT);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&pptr, &vSize)){
		return false;
	}
	copyright = pptr;
	return true;
}


bool ProjectRCVersionString::GetDebug(AnsiString &debug)
{
	if (!FVData)
		return false;

	UINT    vSize;
	TCHAR    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_SPECIALBUILD);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&pptr, &vSize)){
		return false;
	}
	debug = pptr;
	return true;
}

#if 0
// Version comparison for application.
int CompareVersion(const TCHAR *v1, const TCHAR *v2)
{
	const int NUM_DIGIT	= 3;
	int n1[NUM_DIGIT] = {0,0,0};
	int n2[NUM_DIGIT] = {0,0,0};

	if(EOF == _stscanf(v1, _T("%d.%d.%d"), &n1[0], &n1[1], &n1[2])){
		// format error. assume to be zero.
	}
	if(EOF == _stscanf(v2, _T("%d.%d.%d"), &n2[0], &n2[1], &n2[2])){
		// format error. assume to be zero. 
	}

	for(int i=0;i<NUM_DIGIT;i++){
		if(n1[i] < n2[i]){
			return -1;	// n1 is older than n2.
		} else if(n1[i] == n2[i]){
			continue;
		} else {
			return 1;	// n1 is newer than n2.
		}
	}
	return 0;	// Same version.
}
#endif

