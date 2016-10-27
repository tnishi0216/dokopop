#include <windows.h>
#include <tchar.h>

#if WINVER<0x0601
#define QDC_ALL_PATHS                   0x00000001
#define QDC_ONLY_ACTIVE_PATHS           0x00000002
#define QDC_DATABASE_CURRENT            0x00000004
typedef enum
{
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER                   = -1,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15                    =  0,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO                  =  1,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO         =  2,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO         =  3,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI                     =  4,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI                    =  5,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS                    =  6,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN                   =  8,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI                     =  9,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL    = 10,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED    = 11,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL            = 12,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED            = 13,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE              = 14,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL                = 0x80000000,
    DISPLAYCONFIG_OUTPUT_TECHNOLOGY_FORCE_UINT32            = 0xFFFFFFFF
} DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY;
typedef enum
{
    DISPLAYCONFIG_ROTATION_IDENTITY     = 1,
    DISPLAYCONFIG_ROTATION_ROTATE90     = 2,
    DISPLAYCONFIG_ROTATION_ROTATE180    = 3,
    DISPLAYCONFIG_ROTATION_ROTATE270    = 4,
    DISPLAYCONFIG_ROTATION_FORCE_UINT32 = 0xFFFFFFFF
} DISPLAYCONFIG_ROTATION;
typedef enum
{
    DISPLAYCONFIG_SCALING_IDENTITY                  = 1,
    DISPLAYCONFIG_SCALING_CENTERED                  = 2,
    DISPLAYCONFIG_SCALING_STRETCHED                 = 3,
    DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX    = 4,
    DISPLAYCONFIG_SCALING_CUSTOM                    = 5,
    DISPLAYCONFIG_SCALING_PREFERRED                 = 128,
    DISPLAYCONFIG_SCALING_FORCE_UINT32              = 0xFFFFFFFF
} DISPLAYCONFIG_SCALING;
typedef struct DISPLAYCONFIG_RATIONAL
{
    UINT32    Numerator;
    UINT32    Denominator;
} DISPLAYCONFIG_RATIONAL;
typedef enum
{
	DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED                 = 0,
	DISPLAYCONFIG_SCANLINE_ORDERING_PROGRESSIVE                 = 1,
	DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED                  = 2,
	DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED_UPPERFIELDFIRST  = DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED,
	DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED_LOWERFIELDFIRST  = 3,
	DISPLAYCONFIG_SCANLINE_ORDERING_FORCE_UINT32                = 0xFFFFFFFF
} DISPLAYCONFIG_SCANLINE_ORDERING;
typedef enum
{
    DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE = 1,
    DISPLAYCONFIG_MODE_INFO_TYPE_TARGET = 2,
    DISPLAYCONFIG_MODE_INFO_TYPE_FORCE_UINT32 = 0xFFFFFFFF
} DISPLAYCONFIG_MODE_INFO_TYPE;
typedef struct DISPLAYCONFIG_2DREGION
{
    UINT32 cx;
    UINT32 cy;
} DISPLAYCONFIG_2DREGION;
typedef struct DISPLAYCONFIG_VIDEO_SIGNAL_INFO
{
	UINT64                          pixelRate;
	DISPLAYCONFIG_RATIONAL          hSyncFreq;
	DISPLAYCONFIG_RATIONAL          vSyncFreq;
	DISPLAYCONFIG_2DREGION          activeSize;
	DISPLAYCONFIG_2DREGION          totalSize;
	UINT32                          videoStandard;
	DISPLAYCONFIG_SCANLINE_ORDERING scanLineOrdering;
} DISPLAYCONFIG_VIDEO_SIGNAL_INFO;
typedef enum
{
    DISPLAYCONFIG_PIXELFORMAT_8BPP          = 1,
    DISPLAYCONFIG_PIXELFORMAT_16BPP         = 2,
    DISPLAYCONFIG_PIXELFORMAT_24BPP         = 3,
    DISPLAYCONFIG_PIXELFORMAT_32BPP         = 4,
    DISPLAYCONFIG_PIXELFORMAT_NONGDI        = 5,
    DISPLAYCONFIG_PIXELFORMAT_FORCE_UINT32  = 0xffffffff
} DISPLAYCONFIG_PIXELFORMAT;
typedef enum
{
      DISPLAYCONFIG_TOPOLOGY_INTERNAL       = 0x00000001,
      DISPLAYCONFIG_TOPOLOGY_CLONE          = 0x00000002,
      DISPLAYCONFIG_TOPOLOGY_EXTEND         = 0x00000004,
      DISPLAYCONFIG_TOPOLOGY_EXTERNAL       = 0x00000008,
      DISPLAYCONFIG_TOPOLOGY_FORCE_UINT32   = 0xFFFFFFFF
} DISPLAYCONFIG_TOPOLOGY_ID;
typedef enum
{
      DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME             = 1,
      DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME             = 2,
      DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_PREFERRED_MODE   = 3,
      DISPLAYCONFIG_DEVICE_INFO_GET_ADAPTER_NAME            = 4,
      DISPLAYCONFIG_DEVICE_INFO_SET_TARGET_PERSISTENCE      = 5,
      DISPLAYCONFIG_DEVICE_INFO_FORCE_UINT32                = 0xFFFFFFFF
} DISPLAYCONFIG_DEVICE_INFO_TYPE;

typedef struct DISPLAYCONFIG_PATH_SOURCE_INFO
{
	LUID    adapterId;
	UINT32  id;
	UINT32  modeInfoIdx;
	UINT32  statusFlags;
} DISPLAYCONFIG_PATH_SOURCE_INFO;
typedef struct DISPLAYCONFIG_PATH_TARGET_INFO
{
	LUID                                    adapterId;
	UINT32                                  id;
	UINT32                                  modeInfoIdx;
	DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY   outputTechnology;
	DISPLAYCONFIG_ROTATION                  rotation;
	DISPLAYCONFIG_SCALING                   scaling;
	DISPLAYCONFIG_RATIONAL                  refreshRate;
	DISPLAYCONFIG_SCANLINE_ORDERING         scanLineOrdering;
	BOOL                                    targetAvailable;
	UINT32                                  statusFlags;
} DISPLAYCONFIG_PATH_TARGET_INFO;
typedef struct DISPLAYCONFIG_PATH_INFO
{
	DISPLAYCONFIG_PATH_SOURCE_INFO  sourceInfo;
	DISPLAYCONFIG_PATH_TARGET_INFO  targetInfo;
	UINT32                          flags;
} DISPLAYCONFIG_PATH_INFO;
typedef struct DISPLAYCONFIG_TARGET_MODE
{
    DISPLAYCONFIG_VIDEO_SIGNAL_INFO   targetVideoSignalInfo;
} DISPLAYCONFIG_TARGET_MODE;
typedef struct DISPLAYCONFIG_SOURCE_MODE
{
    UINT32                      width;
    UINT32                      height;
    DISPLAYCONFIG_PIXELFORMAT   pixelFormat;
    POINTL                      position;
} DISPLAYCONFIG_SOURCE_MODE;
typedef struct DISPLAYCONFIG_MODE_INFO
{
	DISPLAYCONFIG_MODE_INFO_TYPE    infoType;
	UINT32                          id;
	LUID                            adapterId;
	union
	{
		DISPLAYCONFIG_TARGET_MODE   targetMode;
		DISPLAYCONFIG_SOURCE_MODE   sourceMode;
	};
} DISPLAYCONFIG_MODE_INFO;
typedef struct DISPLAYCONFIG_DEVICE_INFO_HEADER
{
	DISPLAYCONFIG_DEVICE_INFO_TYPE  type;
	UINT32                          size;
	LUID                            adapterId;
	UINT32                          id;
} DISPLAYCONFIG_DEVICE_INFO_HEADER;

#endif

typedef WINUSERAPI LONG (WINAPI *FNGetDisplayConfigBufferSizes)(UINT32 flags, UINT32* numPathArrayElements, UINT32* numModeInfoArrayElements);
typedef WINUSERAPI LONG (WINAPI *FNSetDisplayConfig)(UINT32 numPathArrayElements, DISPLAYCONFIG_PATH_INFO* pathArray, UINT32 numModeInfoArrayElements, DISPLAYCONFIG_MODE_INFO* modeInfoArray, UINT32 flags);
typedef WINUSERAPI LONG (WINAPI *FNQueryDisplayConfig)(UINT32 flags, UINT32* numPathArrayElements, DISPLAYCONFIG_PATH_INFO* pathArray, UINT32* numModeInfoArrayElements, DISPLAYCONFIG_MODE_INFO* modeInfoArray, DISPLAYCONFIG_TOPOLOGY_ID* currentTopologyId);
typedef WINUSERAPI LONG (WINAPI *FNDisplayConfigGetDeviceInfo)(DISPLAYCONFIG_DEVICE_INFO_HEADER* requestPacket);

class TDllHandle {
protected:
	HINSTANCE hInst;
public:
	TDllHandle(HINSTANCE _hInst)
		:hInst(_hInst)
	{
	}
	~TDllHandle(){
		if (hInst) FreeLibrary(hInst);
	}
	HINSTANCE inst(){ return hInst; }
	operator HINSTANCE (){ return hInst; }
	FARPROC GetProcAddress(LPCSTR name)
		{ return ::GetProcAddress(hInst, name); }
};

static TDllHandle *dllHandle = NULL;
static FNGetDisplayConfigBufferSizes _GetDisplayConfigBufferSizes;
static FNQueryDisplayConfig _QueryDisplayConfig;
static bool called = false;
static HMONITOR hPrevMonitor = NULL;
static int prevDpi = 0;
class TDestructor {
public:
	~TDestructor(){ if (dllHandle){ delete dllHandle; dllHandle = NULL; } }
} destructor;

int GetMonitorScale()
{
	int dpi = 96;

	if (!called){
		called = true;
		HINSTANCE hDll = LoadLibrary( _T("user32") );
		if (!hDll)
			return dpi;

		TDllHandle hInst(hDll);
		dllHandle = new TDllHandle(hDll);

		_GetDisplayConfigBufferSizes = (FNGetDisplayConfigBufferSizes)dllHandle->GetProcAddress("GetDisplayConfigBufferSizes");
		if (!_GetDisplayConfigBufferSizes) return dpi;
		_QueryDisplayConfig = (FNQueryDisplayConfig)dllHandle->GetProcAddress("QueryDisplayConfig");
		if (!_QueryDisplayConfig) return dpi;
	}

	if (!dllHandle) return dpi;

	POINT pt;
	GetCursorPos( &pt );
	HMONITOR hMonitor = MonitorFromPoint( pt, MONITOR_DEFAULTTONULL );
	if (!hMonitor) return dpi;

	if (hMonitor == hPrevMonitor){
		return prevDpi;
	}
	hPrevMonitor = hMonitor;

	MONITORINFOEX LogicalMonitorInfo;
	LogicalMonitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &LogicalMonitorInfo);
	int LogicalMonitorWidth = LogicalMonitorInfo.rcMonitor.right - LogicalMonitorInfo.rcMonitor.left;

	int LogicalDesktopWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	UINT32 numofpath;
	UINT32 numofmode;
	_GetDisplayConfigBufferSizes(QDC_DATABASE_CURRENT, &numofpath, &numofmode);

	DISPLAYCONFIG_PATH_INFO *dpis = new DISPLAYCONFIG_PATH_INFO[numofpath];
	DISPLAYCONFIG_MODE_INFO *pModeInfoArray = new DISPLAYCONFIG_MODE_INFO[numofmode];
	DISPLAYCONFIG_TOPOLOGY_ID tid;
	_QueryDisplayConfig(QDC_DATABASE_CURRENT, &numofpath, dpis, &numofmode, pModeInfoArray, &tid);	//TODO: Windows7 or later

	int PhysicalDesktopWidth = LogicalDesktopWidth;
	int PhysicalMonitorWidth = LogicalMonitorWidth;
	
	for (int i=0;i<(int)numofmode;i++){
		if (pModeInfoArray[i].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE){
			//DBW("%d: %d", i, pModeInfoArray[i].sourceMode.width);
			PhysicalDesktopWidth = pModeInfoArray[i].sourceMode.width;
			//int PhysicalDesktopWidth = 
			//int ScaleFactor = (LogicalMonitorWidth/LogicalDesktopWidth) / (PhysicalMonitorWidth/PhysicalDesktopWidth)
			//return 96 * 1;
		} else
		if (pModeInfoArray[i].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_TARGET){
			PhysicalMonitorWidth = pModeInfoArray[i].targetMode.targetVideoSignalInfo.activeSize.cx;
			//DBW("%d: %d", i, PhysicalMonitorWidth);
		}
	}

	//int PhysicalMonitorWidth = pModeInfoArray[i].sourceMode.width;
	//int ScaleFactor = (LogicalMonitorWidth/LogicalDesktopWidth) / (PhysicalMonitorWidth/PhysicalDesktopWidth)

	delete[] dpis;
	delete[] pModeInfoArray;

	return prevDpi = dpi * PhysicalDesktopWidth / LogicalDesktopWidth;
}
