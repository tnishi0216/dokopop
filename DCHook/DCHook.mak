# Microsoft Developer Studio Generated NMAKE File, Based on DCHook.dsp

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.\.
# End Custom Macros


ALL : "$(OUTDIR)\DCHook.dll"



CLEAN :
	-@erase "$(INTDIR)\dchook.res"
	-@erase "$(INTDIR)\dchook.obj"
	-@erase "$(INTDIR)\dchook.sbr"
	-@erase "$(OUTDIR)\DCHook.dll"
	-@erase "$(OUTDIR)\DCHook.exp"
	-@erase "$(OUTDIR)\DCHook.lib"
	-@erase "$(OUTDIR)\DCHook.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W4 /GX /G4 /GA /O1 /I "$(MSAADEV)\inc32" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\DCHook.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=./
CPP_SBRS=./
MTL_PROJ=/nologo /I "$(MSAADEV)\inc32" /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dchook.res" /i "$(MSAADEV)\inc32" /d "NDEBUG" 

LINK32=link.exe
LINK32_FLAGS=user32.lib kernel32.lib gdi32.lib advapi32.lib \
 /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\DCHook.pdb" /map:"$(INTDIR)\DCHook.map" /machine:I386\
 /def:".\dchook.def" /out:"$(OUTDIR)\DCHook.dll"\
 /implib:"$(OUTDIR)\DCHook.lib" /libpath:"$(MSAADEV)\lib" 
DEF_FILE= \
	".\dchook.def"
LINK32_OBJS= \
	"$(INTDIR)\dchook.res" \
	"$(INTDIR)\dchook.obj"

"$(OUTDIR)\DCHook.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

SOURCE=.\dchook.rc

"$(INTDIR)\dchook.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)

SOURCE=.\dchook.cpp
DEP_CPP_KEYS_= "DCHook.h"

"$(INTDIR)\dchook.obj"	: $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"

