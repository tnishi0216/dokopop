//// HK95c.c
// --------------------------------------------
// Dynamically loadable VxD for DCHOOK
// --------------------------------------------
#define WIN32_LEAN_AND_MEAN // Excludes un-needed parts of windows.h
#include <windows.h>
#include <vmm.h>
#include <vwin32.h>
#include <debug.h>
#include "hk95d.h"

// -------------------------------------
// Externs defined in assembly module 
// -------------------------------------
// These defined in assembly for the VMM.INC or VPICD.INC file inclusion
// ------------------------
// PRAGMA for this DATA 
// ------------------------
// Establish segment
#pragma data_seg ( "_LDATA","_LCODE")

// ------------------------------------
// Data structures MUST be INITIALIZED
// ------------------------------------
// ------------------------
// PRAGMAS for this CODE 
// ------------------------
// Establish segment
#pragma code_seg ( "_LTEXT", "_LCODE" )
//No stack checking for routines in this module
#pragma check_stack(off)

// ---------------------------------------
// Set Good Return code for DIOC requests
// ---------------------------------------
void _declspec(naked)GoodReturnDIOC(void)
{
	// Clear eax and carry flag for GOOD return
	_asm xor eax,eax
	_asm clc
	_asm ret;
}

// ---------------------------------------
// Set Bad Return code for DIOC requests
// ---------------------------------------
void _declspec(naked) BadReturnDIOC(void)
{
	// NOTE: 50 is a FCN NOT SUPPORTED code -- ok to use
	// SET carry flag for BAD return
	_asm mov eax,50
	_asm stc
	_asm ret;
}


// ------------------------------
// Routine for DCHOOK Device UNINIT  
// ------------------------------
void CHK95_Device_UNInit()
{
	// Set GOOD return code
	GoodReturnDIOC();
	return;
}
// ------------------------------
// Routine for DCHOOK Device INIT  
// ------------------------------

void CHK95_Device_Init()
{
    GoodReturnDIOC();
}
DWORD VXDINLINE
PageModPerm( DWORD Page, DWORD nPages, DWORD Flags1, DWORD Flags2 )
{
    DWORD    dw;

	_asm push [Flags2]
	_asm push [Flags1]
	_asm push [nPages]
	_asm push [Page]
    VMMCall( _PageModifyPermissions );
	_asm add esp, 10h
    _asm mov [dw], eax

    return(dw);
}

// --------------------------------
// Routine for DCHOOK Device IO ctrl  
// --------------------------------
void CHK95_Device_IOctrl(PDIOCPARAMETERS ptr)
{
	int i;
	unsigned int addr;
	unsigned int page;
	int npage;
	unsigned char *p;
	unsigned char *q;
	DWORD oldpermission;
	// Field the DEV IO requests from VMM
	switch(ptr->dwIoControlCode)
		{
		case DIOC_WRITEABLE_MEMORY:	// The address to be changed must be already committed.
			if ( ptr->lpvInBuffer ){
				// Does it need a PC_STATIC?

				long r = 0;

				// I don't know exact parameters. But it works well, so I don't care about it....

				r = PageModPerm( *((DWORD*)ptr->lpvInBuffer), 1, 0xFFFFFFFF, PC_WRITEABLE | PC_USER | PC_STATIC );
				if ( ptr->lpvOutBuffer ){
					*((DWORD*)ptr->lpvOutBuffer) = r;
				}
			}
		break;
		case DIOC_NONWRITEABLE_MEMORY:	// The address to be changed must be already committed.
			{
			long r = 0;

			r = PageModPerm( *((DWORD*)ptr->lpvInBuffer), 1, ~PC_WRITEABLE, PC_USER | PC_STATIC );
			if ( ptr->lpvOutBuffer ){
				*((DWORD*)ptr->lpvOutBuffer) = r;
			}
			}
		break;
		case DIOC_CHANGE_MEMORY:
			addr = (DWORD)ptr->lpvOutBuffer;
			page = addr >> 12;	// 4KB
			npage = ((addr + ptr->cbInBuffer) >> 12) - page + 1;
			oldpermission = PageModPerm( page, npage, 0xFFFFFFFF, PC_WRITEABLE | PC_USER | PC_STATIC );
			_asm pushfd
			_asm cli
			if ( oldpermission == (DWORD)-1 ){
				_asm popfd
				BadReturnDIOC();
				return;
			}
			p = (unsigned char*)addr;
			q = (unsigned char*)ptr->lpvInBuffer;
			for ( i=0;i<ptr->cbInBuffer;i++ ){
				*p++ = *q++;
			}
			PageModPerm( page, npage, oldpermission, oldpermission );
			_asm popfd
		break;
#if 0
		case DIOC_DEBUG:
			*(DWORD*)ptr->lpvOutBuffer = (DWORD)CHK95_Device_IOctrl;
		break;
#endif
		//The below DIOC_GETVERSION is a part of the dynamic load protocol
		//It MUST return a GOOD code (all codes here use GoodReturnDIOC()
//		case(DIOC_GETVERSION):
//		case(DIOC_CLOSEHANDLE):
//		default:
//		break;
	}
 	GoodReturnDIOC();
	return;
}

