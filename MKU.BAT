@echo off
echo/
echo 完全版パッケージを作成します
echo/
echo ■確認事項
echo ・./dchooktest/dchooktest.exeを準備(USE_UNICODE defined)
echo ・バージョン番号：.bpr .tpl の変更が必要
echo ・Release Build?
pause
set TZ=JST-9

echo/
echo Copy the binary to the ind directory.
echo/
pause

make copy_bin

rem touch -d06/12/26 -t03:00:01 -c ind\DKPU.EXE ind\DKPUHK.DLL

rem dkpu.issの更新
perl change_iss.pl ind\dkpu.iss Setup AppVerName "DokoPop! 3.0.1" > ind\dkpu.iss.new
del ind\dkpu.iss.old
ren ind\dkpu.iss dkpu.iss.old
ren ind\dkpu.iss.new dkpu.iss

rem Installer の実行
make buildis

echo/
echo Press Enter key after installer made.
echo/
pause

copy ind\Output\setup.EXE DKPU301.exe
dir DKPU301.exe

rem lha32 a DKPU301.exe DKPU.TXT DKPU.EXE DKPUHK.DLL
ren DKPU301.exe DKPU301.exe
rem touch -d06/12/26 -t03:00:01 -c DKPU301.exe

sha2.py DKPU301.exe > DKPU301.exe.sha256

echo/
echo ソースファイルをzipします
echo/
pause

make zip_src
copy src.zip src-3.0.1.zip
rem copy src-3.0.1.zip zip

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

rem copy DKPU301.exe S:\Web\NIFTY\
rem del S:\Web\NIFTY\DKPU300.EXE
copy DKPU301.exe S:\web\sakura-pdic\pub\
copy DKPU301.exe.sha256 S:\web\sakura-pdic\pub\
del S:\web\sakura-pdic\pub\DKPU300.EXE
del S:\web\sakura-pdic\pub\DKPU300.EXE.sha256

rem echo [del] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU300.EXE >> S:\Web\NIFTY\ftp.ctl
rem echo [include] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU301.exe >> S:\Web\NIFTY\ftp.ctl

