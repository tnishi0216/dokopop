@echo off
echo/
echo 完全版パッケージを作成します
echo/
echo ■確認事項
echo ・./dchooktest/dchooktest.exeを準備(USE_UNICODE defined)
echo ・バージョン番号：.bpr、Util.cpp(StrVersion, VersionValue)と.tplが必要
echo ・Release Build?
echo ・VisualStudio2008でbuildしたか？ for W2K
pause
set TZ=JST-9

echo/
echo Copy the binary to the ind directory.
echo/
pause

make copy_bin

rem touch -d08/21/21 -t02:01:03 -c ind\DKPU.EXE ind\DKPUHK.DLL

rem dkpu.issの更新
perl change_iss.pl ind\dkpu.iss Setup AppVerName "DokoPop! 2.1.4" > ind\dkpu.iss.new
del ind\dkpu.iss.old
ren ind\dkpu.iss dkpu.iss.old
ren ind\dkpu.iss.new dkpu.iss

rem Installer の実行
make buildis

echo/
echo Press Enter key after installer made.
echo/
pause

copy ind\Output\setup.EXE DKPU214.exe
dir DKPU214.exe

rem lha32 a DKPU214.exe DKPU.TXT DKPU.EXE DKPUHK.DLL
ren DKPU214.exe DKPU214.exe
rem touch -d08/21/21 -t02:01:03 -c DKPU214.exe

echo/
echo ソースファイルをzipします
echo/
pause

make zip_src
copy src.zip src-2.1.4.zip
rem copy src-2.1.4.zip zip

echo/
echo 公開用ファイルを更新します
echo/
rem pause

perl -S -i.bak filestamp.pl DKPUu.TXT
del DKPUu.TXT.bak

echo/
echo Web用のファイルを更新します
echo/
pause

rem copy DKPU214.exe S:\Web\NIFTY\
rem del S:\Web\NIFTY\DKPU213.EXE
copy DKPU214.exe S:\web\sakura-pdic\cgi-bin\download\files\
del S:\web\sakura-pdic\cgi-bin\download\files\DKPU213.EXE

rem echo [del] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU213.EXE >> S:\Web\NIFTY\ftp.ctl
rem echo [include] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU214.exe >> S:\Web\NIFTY\ftp.ctl

