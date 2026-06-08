#ifndef	__PDVER_H
#define	__PDVER_H

// VERSIONINFO リソースを読む
class ProjectRCVersion {
public:
    ProjectRCVersion ( const TCHAR *appFName );	// appFNameは TModule::GetModuleFileName()で得られる
    virtual ~ProjectRCVersion ();
	virtual bool GetProductVersion(AnsiString &prodVersion);
	virtual unsigned GetProductVersionValue();

protected:
	void *FVData;

private:
	// このオブジェクトはコピーしてはならない
	ProjectRCVersion (const ProjectRCVersion &);
	ProjectRCVersion & operator =(const ProjectRCVersion &);
};

// Complicated version.
class ProjectRCVersionString : public ProjectRCVersion {
typedef ProjectRCVersion super;
protected:
	LPBYTE      TransBlock;
public:
	ProjectRCVersionString(const TCHAR *appFName);
	bool GetProductName(AnsiString &prodName);
	bool GetCopyright (AnsiString &copyright);
	bool GetDebug (AnsiString &debug);
};

int CompareVersion(const TCHAR *v1, const TCHAR *v2);

#endif	__PDVER_H

