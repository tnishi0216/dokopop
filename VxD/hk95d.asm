;;HK95d.asm
name HK95D
.386p
;; ------------------------------------------------
;; ------------------------------------------------
;; HK95d.asm -- Assembly module, MAIN module for
;; Dynamically loadable VxD for DCHOOK
;; ------------------------------------------------
;; ------------------------------------------------

;; -----------------------------------
;; INCLUDE files needed by this module
;; -----------------------------------

include <inc\win98\vmm.inc>
include <inc\win98\vwin32.inc>
include <inc\win98\vpicd.inc>

;; ------------------------------------
;; C routines/data used by this module
;; ------------------------------------
extrn _CHK95_Device_Init:near
extrn _CHK95_Device_UNInit:near
extrn _CHK95_Device_IOctrl:near

;; -------------------------------------
;; Routines/data called from this module
;; -------------------------------------
; public

 ; ==================================================================================
;; Misc HK95-specific Equates
;; ----------------------------------------------------------------------------------
HK95_MajoREV equ 1		 ;MAJOR revision level
HK95_MinoREV equ 0		 ;decimal number of revision
HK95_DeviceID equ Undefined_Device_ID ;no need for MS device number assignment
; ==================================================================================

;; ---------------------------------------------
;; Virtual Device Declaration (Required)
;; (Declares this code as virtual device driver)
;; Also creates the Device Data Block
;; ---------------------------------------------
 
Declare_Virtual_Device HK95D,HK95_MajoREV,HK95_MinoREV,HK95_Control,HK95_DeviceID,Undefined_Init_Order

VxD_LOCKED_CODE_SEG
;; --------------------------------------------
;; Control Dispatch Table & Proc (Required)
;; Used to dispatch supported messages sent by
;; VMM -- clears carry for unsupported mssgs.
;; --------------------------------------------
;; Only 3 VMM messages are recognized and processed
;; by this routine -- all DIOC interface messages
;; translate to W32_DeviceIoControl mssgs from the VMM.
;; "Control_Dispatch" precedes MSSG NUMBER, PROCEDURE

BeginProc HK95_Control
    Control_Dispatch Sys_Dynamic_Device_Exit, HK95_Device_UNInit
    Control_Dispatch Sys_Dynamic_Device_Init, HK95_Device_Init
	Control_Dispatch W32_DeviceIoControl,     HK95_Device_IOctrl
    xor eax,eax  ;;return 0 (required in some instances)
    clc          ;;clear carry flg for GOOD indicator   
	ret
EndProc HK95_Control

;; -------------------------------------------------------------
;; NOTE: "BeginProc & EndProc" are needed in conjunction with
;; the above dispatch table -- below routines facilitate C fcns
;; -------------------------------------------------------------

;; =======================================================================
;; Routines below are VXD interface (load, unload, process) ROUTINES
;; =======================================================================

;; --------------------------------------------
;; Routine to jump to C routine for processing
;; SYS_DYNAMIC_DEVICE_INIT message
;; --------------------------------------------
BeginProc HK95_Device_Init
    call _CHK95_Device_Init
    ret
EndProc HK95_Device_Init

;; --------------------------------------------
;; Routine to jump to C routine for processing
;; SYS_DYNAMIC_DEVICE_EXIT message
;; --------------------------------------------

BeginProc HK95_Device_UNInit
        call _CHK95_Device_UNInit
        ret
EndProc HK95_Device_UNInit

;; --------------------------------------------
;; Routine to jump to C routine for processing
;; W32_DEVICEIOCONTROL messages -- These are
;; VxD requests from the application.
;; At entry, esi points to the DIOC interface
;; structure passed by the application
;; --------------------------------------------

BeginProc HK95_Device_IOctrl
        push esi
        call _CHK95_Device_IOctrl
        pop esi
        ret
EndProc HK95_Device_IOctrl

;; ======================================================
;; Routines below are miscellaneous assembly interfaces
;; ======================================================

VxD_LOCKED_CODE_ENDS

VxD_LOCKED_DATA_SEG
VxD_LOCKED_DATA_ENDS

        END

