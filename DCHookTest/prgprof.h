//---------------------------------------------------------------------------

#ifndef prgprofH
#define prgprofH
//---------------------------------------------------------------------------

#ifdef	DEFINE_PFS
#define	DEF_PFS(name, str)	extern const char PFS_##name[]; const char PFS_##name[] = str
#else
#define	DEF_PFS(name, str)	extern const char PFS_##name[]
#endif

DEF_PFS(MAIN, "Main");
DEF_PFS(CONFIG, "Config");
DEF_PFS(PDIC, "PDIC");
DEF_PFS(COMMON, "Common");
DEF_PFS(GRPSEL, "grpsel");
DEF_PFS(GROUP, "Group");
DEF_PFS(GROUPOPEN, "GroupOpen");
DEF_PFS(GROUPNAME, "GroupName");
DEF_PFS(POPUPKEY, "PopupKey");
DEF_PFS(TOGGLEKEY, "ToggleKey");
DEF_PFS(CTRLCLOSE, "CtrlClose");
DEF_PFS(IGNOREJ, "IgnoreJ");
DEF_PFS(PATH, "Path");
DEF_PFS(BANNER, "Banner");
DEF_PFS(INCSRCH, "IncSrch");
DEF_PFS(ADVANCED, "Advanced");
DEF_PFS(CAPTURE_MODE, "CaptureMode2");
DEF_PFS(DPI_DETECT, "DPIDetect");
DEF_PFS(SCALE, "Scale");
DEF_PFS(USE64, "Use64");

extern class TMyIni *Ini;

#endif

