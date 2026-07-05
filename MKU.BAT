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

rem touch -d07/05/26 -t03:00:02 -c ind\DKPU.EXE ind\DKPUHK.DLL

rem dkpu.issの更新
change_iss.py ind\dkpu.iss Setup AppVerName "DokoPop! 3.0.2" > ind\dkpu.iss.new
del ind\dkpu.iss.old
ren ind\dkpu.iss dkpu.iss.old
ren ind\dkpu.iss.new dkpu.iss

rem Installer の実行
make buildis

echo/
echo Press Enter key after installer made.
echo/
pause

copy ind\Output\setup.EXE DKPU302.exe
dir DKPU302.exe

rem lha32 a DKPU302.exe DKPU.TXT DKPU.EXE DKPUHK.DLL
ren DKPU302.exe DKPU302.exe
rem touch -d07/05/26 -t03:00:02 -c DKPU302.exe

sha2.py DKPU302.exe > DKPU302.exe.sha256

echo/
echo 公開用ファイルを更新します
echo/
rem pause

rem perl -S -i.bak filestamp.pl DKPUu.TXT
del DKPUu.TXT.bak

echo/
echo Web用のファイルを更新します
echo/
pause

rem 正式版後有効
if not exist deploydev (
	deploy.py dokopop -rc:DCHookTest\DCHookTest.bpr -setup:DKPU302.exe -skip:setupcopy -sha2 @sakura
)

pause

rem copy DKPU302.exe S:\Web\NIFTY\
rem del S:\Web\NIFTY\DKPU301.EXE
copy DKPU302.exe S:\web\sakura-pdic\pub\
copy DKPU302.exe.sha256 S:\web\sakura-pdic\pub\
del S:\web\sakura-pdic\pub\DKPU301.EXE
del S:\web\sakura-pdic\pub\DKPU301.EXE.sha256

rem echo [del] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU301.EXE >> S:\Web\NIFTY\ftp.ctl
rem echo [include] >> S:\Web\NIFTY\ftp.ctl
rem echo DKPU302.exe >> S:\Web\NIFTY\ftp.ctl

echo/
echo ソースファイルをzipします
echo/
pause

make zip_src
copy src.zip src-3.0.2.zip
rem copy src-3.0.2.zip zip

