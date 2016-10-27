//---------------------------------------------------------------------------
#include <windows.h>
#include <oleidl.h>
#pragma hdrstop

//---------------------------------------------------------------------------

const GUID CLSID_Document = 
	{0x40942a6c,0x1520,0x4132,{0xbd,0xf8,0xbd,0xc1,0xf7,0x1f,0x54,0x7b}}; 

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
		const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

MIDL_DEFINE_GUID(IID, IID_IDocument,0xD4073843,0xA58A,0x469A,0xA8,0xE2,0xCF,0xF3,0xFF,0x77,0xEE,0x4E);

#define	_Inout_
#define	_In_
#define	_Out_
#define	_Deref_out_opt_

struct __POSITION {};
typedef __POSITION* POSITION;

typedef /* [v1_enum] */ 
enum WTS_ALPHATYPE
	{	WTSAT_UNKNOWN	= 0,
	WTSAT_RGB	= 1,
	WTSAT_ARGB	= 2
	} 	WTS_ALPHATYPE;

class IFilterChunkValue;
	
interface IDocument
{
protected:
	IDocument() : m_refCount(0)
	{
	}

public:
	virtual ~IDocument()
	{
	}

	virtual LONG AddRef()
	{
		return InterlockedIncrement((long*)&m_refCount);
	}

	virtual LONG Release()
	{
		//__assert__(m_refCount > 0);
		if (InterlockedDecrement((long*)&m_refCount) == 0)
		{
			delete this;
			return 0;
		}

		return m_refCount;
	}

	virtual HRESULT LoadFromStream(
		_Inout_ IStream* pStream,
		_In_ DWORD grfMode) = 0;
	virtual void InitializeSearchContent() = 0;
	virtual void BeginReadChunks() = 0;

	virtual BOOL GetThumbnail(
		_In_ UINT cx,
		_Out_ HBITMAP* phbmp,
		_Out_ WTS_ALPHATYPE* pdwAlpha) = 0;

	virtual void ClearChunkList() = 0;
	virtual BOOL SetChunkValue(
		_In_ IFilterChunkValue* value) = 0;
	virtual BOOL ReadNextChunkValue(
		_Deref_out_opt_ IFilterChunkValue** value) = 0;
	virtual void RemoveChunk(
		_In_ REFCLSID guid,
		_In_ DWORD pid) = 0;
	virtual POSITION FindChunk(
		_In_ REFCLSID guid,
		_In_ DWORD pid) = 0;

	virtual LPVOID GetContainer() const = 0;

protected:
	volatile LONG m_refCount;
};

#pragma argsused
bool MODIAvail();
int main(int argc, char* argv[])
{
	if (MODIAvail())
		return 1;
	return 0;
}

bool MODIAvail()
{
	bool exist = false;
	HRESULT hr = CoInitialize(0);
	if (SUCCEEDED(hr)){
		IDocument *IDobj = NULL;
		hr = ::CoCreateInstance(CLSID_Document, NULL, CLSCTX_ALL, IID_IDocument, (void**)&IDobj);
		if (SUCCEEDED(hr)){
#if 0
			hr = IDobj->Create(AsciiToBSTR("H:\\temp\\nishikawat\\amodi\\2014-12-02-151156.bmp"));
			if (SUCCEEDED(hr){
				
			}
#endif
			exist = true;
			IDobj->Release();	//TODO: stack‚ð‰ó‚µ‚Ä‚µ‚Ü‚¤
		}
	}
	CoUninitialize();

	return exist;
}


