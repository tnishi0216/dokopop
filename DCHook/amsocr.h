#ifndef __amsocr_h
#define	__amsocr_h

#define	WMCD_EXISTCHECK		0x4000
#define	WMCD_SETPOINT		0x4001

#define	WM_AMSOCR				(WM_APP+0x400)	// app communication message with AMSOCR
#define	WM_MOVESEND				(WM_APP+0x208)	// DCH_MOVESENDの代わりにPostMessageで送る
#define	AMSOCR_CMD_QUERY			0
#define	AMSOCR_CMD_PAGE_CAPTURE	1

class AMSOCR {
protected:
	HWND hwndAMSOCR = nullptr;
public:	
	HWND GetHandle() const { return hwndAMSOCR; }
	bool IsWindowEnabled() const { return ::IsWindowEnabled(hwndAMSOCR) == TRUE; }
	void ResetHandle() { hwndAMSOCR = nullptr; }
	HWND Find(bool force=false);
	int Send(int cmd, const char *data, int len);
	void CheckAlive();
};


#endif
