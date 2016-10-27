// ExMODIst.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExMODIst.h"
#include "MDIVWCTL.tlh"

using namespace modi;

bool IsMODIAvailable();

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	if (IsMODIAvailable()){
		return 1;
	}
	return 0;
}

bool IsMODIAvailable()
{
	bool bRet= true;

    HRESULT hr;
    modi::IDocument  *IDobj   = NULL;

	hr = CoInitialize(0);
	if ( SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_Document,NULL,CLSCTX_ALL,IID_IDocument,(void**) &IDobj);

		if ( SUCCEEDED(hr) )
		{
			IDobj->Release();
		} else {
		   bRet= false;
		}

	} else {
		bRet= false;
	}

	return bRet;
}
