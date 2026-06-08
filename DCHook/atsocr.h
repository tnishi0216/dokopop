#ifndef __atsocr_h
#define	__atsocr_h

#define	WMCD_EXISTCHECK		0x4000
#define	WMCD_SETPOINT		0x4001

#define	WM_ATSOCR				(WM_APP+0x400)	// app communication message with ATSOCR
#define	WM_MOVESEND				(WM_APP+0x208)	// DCH_MOVESENDの代わりにPostMessageで送る
#define	ATSOCR_CMD_QUERY			0
#define	ATSOCR_CMD_PAGE_CAPTURE	1

class ATSOCR {
protected:
	HWND hwndATSOCR;
public:
	ATSOCR(){ hwndATSOCR = nullptr; }
	HWND GetHandle() const { return hwndATSOCR; }
	bool IsWindowEnabled() const { return ::IsWindowEnabled(hwndATSOCR) == TRUE; }
	void ResetHandle() { hwndATSOCR = nullptr; }
	HWND Find(bool force=false);
	int Send(int cmd, const char *data, int len);
	void CheckAlive();
};


#endif
