//---------------------------------------------------------------------------
#include <windows.h>
#pragma hdrstop
#include "MDIVWCTL.tlh"

//---------------------------------------------------------------------------
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
			IDobj->Release();
		}
	}
	CoUninitialize();

	return exist;
}


