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

rem touch -d03/21/18 -t02:01:03 -c ind\DKPU.EXE ind\DKPUHK.DLL

rem dkpu.issの更新
perl change_iss.pl ind\dkpu.iss Setup AppVerName "DokoPop! 2.1.3" > ind\dkpu.iss.new
del ind\dkpu.iss.old
ren ind\dkpu.iss dkpu.iss.old
ren ind\dkpu.iss.new dkpu.iss

rem Installer の実行
make buildis

echo/
echo Press Enter key after installer made.
echo/
pause

copy ind\Output\setup.EXE DKPU213.exe
dir DKPU213.exe

rem lha32 a DKPU213.exe DKPU.TXT DKPU.EXE DKPUHK.DLL
ren DKPU213.exe DKPU213.exe
rem touch -d03/21/18 -t02:01:03 -c DKPU213.exe

echo/
echo ソースファイルをzipします
echo/
pause

make zip_src
copy src.zip src-2.1.3.zip
rem copy src-2.1.3.zip zip

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

rem copy DKPU213.exe S:\Web\NIFTY\
rem del S:\Web\NIFTY\DKPU212.EXE
copy DKPU213.exe S:\web\sakura-pdic\cgi-bin\download\files\
del S:\web\sakura-pdic\cgi-bin\download\files\DKPU212.EXE

rem echo [del] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU212.EXE >> S:\Web\NIFTY\ftp.ctl
rem echo [include] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU213.exe >> S:\Web\NIFTY\ftp.ctl

