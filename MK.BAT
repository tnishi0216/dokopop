@echo off
echo/
echo 完全版パッケージを作成します
echo/
echo ./dchook/dchook.dllを準備
echo ./dchooktest/dchooktest.exeを準備
echo ./vxd/hk95d.vxdを準備
pause
set TZ=JST-9

copy .\dchook\dchook.dll .\DKPPHK.DLL /y
copy .\VxD\hk95d.vxd .\HK95D.VXD /y
copy .\dchooktest\dchooktest.exe .\DKPP.EXE /y

touch -d12/09/02 -t01:11:00 -c DKPP.EXE DKPPHK.DLL HK95D.VXD README.TXT

del dchook-1.11.lzh > nul

lha32 a dchook-1.11.lzh /x README.TXT
lha32 a dchook-1.11.lzh /x ./dchook/dchook.mak ./dchook/dchook.cpp ./dchook/dchook.h ./dchook/dchook.rc ./dchook/dchook.def
lha32 a dchook-1.11.lzh /x ./dchooktest/dchooktest.bpr ./dchooktest/dchooktest.mak
lha32 a dchook-1.11.lzh /x ./dchooktest/*.h ./dchooktest/*.cpp ./dchooktest/dchooktest.rc ./dchooktest/*.dfm ./dchooktest/dkpp.bmp ./dchooktest/popenable.bmp ./dchooktest/popdisable.bmp ./dchooktest/popenable.h ./dchooktest/popdisable.h ./dchooktest/icon.rc
lha32 a dchook-1.11.lzh /x ./vxd/makefile ./vxd/hk95d.def ./vxd/*.h ./vxd/*.asm ./vxd/*.c

perl -S chgini.pl dchook.ind Exe Name DKPP111.EXE
perl -S chgini.pl dchook.ind Date Year 2002
perl -S chgini.pl dchook.ind Date Month 12
perl -S chgini.pl dchook.ind Date Day 09
perl -S chgini.pl dchook.ind Date Hour 01
perl -S chgini.pl dchook.ind Date Minute 11
perl -S chgini.pl dchook.ind Date Second 00
perl -S chgini.pl dchook.ind Back 8 "|Doko Pop! Ver.1.11"
perl -S chgini.pl dchook.ind Back 12 "|Doko Pop! Ver.1.11"

rem INDの実行
k:\wbin\makeset\makeset .\dchook.ind

rem lha32 a DKPP111.EXE DKPP.TXT DKPP.EXE DKPPHK.DLL HK95D.VXD
ren DKPP111.EXE DKPP111.EXE
touch -d12/09/02 -t01:11:00 -c DKPP111.EXE

echo/
echo 保存用圧縮ファイルを作成します
echo/
rem pause

dir dchook-1.11.lzh

echo/
echo 公開用ファイルを更新します
echo/
rem pause

perl -S -i.bak filestamp.pl DKPPu.TXT
del DKPPu.TXT.bak

echo/
echo Web用のファイルを更新します
echo/
pause

copy dchook-1.11.lzh S:\Web\NIFTY\
copy dchook-1.11.lzh S:\down\MySoft
del S:\Web\NIFTY\dchook-1.10.lzh
copy DKPP111.EXE S:\Web\NIFTY\
copy DKPP111.EXE S:\down\MySoft
del S:\Web\NIFTY\DKPP110.EXE

copy S:\Web\NIFTY\pdic-download.var vars.dat
perl -S repvars.pl S:\Web\NIFTY\pdic-download0.html > S:\Web\NIFTY\pdic-download.html
copy vars.dat S:\Web\NIFTY\pdic-download.var

rem echo [del] >> S:\Web\NIFTY\ftp.ctl
rem echo dchook-1.10.lzh >> S:\Web\NIFTY\ftp.ctl
rem echo DKPP110.EXE >> S:\Web\NIFTY\ftp.ctl
rem echo [include] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPP111.EXE >> S:\Web\NIFTY\ftp.ctl
rem echo dchook-1.11.lzh >> S:\Web\NIFTY\ftp.ctl


