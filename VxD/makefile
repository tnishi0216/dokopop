BUILD_TYPE=base
# !INCLUDE $(BASEDIR)\inc\master.mk

# OUTDIR = obj\i386\\

# sample make file for building vxd virtual device for Windows

NAME = HK95D

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for MASM 6 Assembler

INC32  = k:\98ddk
ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 -I$(INC32) $(DDEBUG)
ASMENV = ML
LFLAGS = /VXD /NOD

CFLAGS = -DWIN32 -Di386 -D_X86_ -I$(BASEDIR)\inc

#       MASM 6 only inference rules

.asm{$(OUTDIR)}.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

.c{$(OUDIR)}.obj:
	$(CC) -c $(CFLAGS) -Fo$*.obj $*.c

all : $(OUTDIR)$(NAME).VXD

OBJS = $(OUTDIR)hk95d.obj $(OUTDIR)hk95c.obj

$(OUTDIR)vxdstub.exe: $(OUTDIR)vxdstub.obj
!ifndef PASS0ONLY
	@echo link -OUT:$@
	$(LINK16) $(OUTDIR)vxdstub.obj;
!endif

$(OUTDIR)hk95d.obj: hk95d.asm

$(OUTDIR)$(NAME).vxd: $(NAME).def $(OBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<$(OUTDIR)$(NAME).lnk
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:$(OUTDIR)$(NAME).vxd
/MAP:$(TOUDIR)$(NAME).map
/DEF:$(NAME).def
$(OBJS)
<<
        mapsym -s -o $(OUTDIR)R$(NAME).sym $(OUTDIR)$(NAME).map
!endif

clean:
    -@if exist $(OUTDIR)*.obj del $(OUTDIR)*.obj
    -@if exist $(OUTDIR)*.vxd del $(OUTDIR)*.vxd
    -@if exist $(OUTDIR)*.exp del $(OUTDIR)*.exp
    -@if exist $(OUTDIR)*.lib del $(OUTDIR)*.lib
    -@if exist $(OUTDIR)*.map del $(OUTDIR)*.map
    -@if exist $(OUTDIR)*.sym del $(OUTDIR)*.sym
    -@if exist *.bak del *.bak
