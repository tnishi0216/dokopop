#!make

CPI=cp -i -p -u

all:

prep:
	-mkdir DCHookTest\obj

copy_release:
	copy DCHook\Release\DCHook.dll DCHookTest\DKPUHK.DLL
#	copy DCHook\Win32\Release\DCHook.dll DCHookTest\DKPUHK.DLL
#	copy DCHook\x64\Release\DCHook.dll DCHookTest\DKPUHK64.DLL
#	copy DCHook64\dchk64\x64\Release\dchk64.exe DCHooktest\DKPUHK64.exe

copy_bin:
	echo DKPUHK.DLL(DCHOOK.DLL)ÇÕVS2008Ç≈çÏê¨ÇµÇΩÇ©ÅH
	pause
#	$(CPI) DCHook\Win32\Release\DCHook.dll ind\DKPUHK.dll
#	$(CPI) DCHook\x64\Release\DCHook.dll ind\DKPUHK64.dll
#	$(CPI) bin\DKPUHK.DLL ind\DKPUHK.dll
#	$(CPI) DCHook64\dchk64\x64\Release\dchk64.exe ind\DKPUHK64.exe
	$(CPI) DCHookTest\DKPUHK.dll ind\DKPUHK.dll
	$(CPI) DCHookTest\DKPUHK64.DLL ind\DKPUHK64.dll
	$(CPI) DCHookTest\DKPUHK64.exe ind\DKPUHK64.exe
	$(CPI) bin\amodi.exe ind\amodi.exe
	$(CPI) bin\Interop.MODI.dll ind\Interop.MODI.dll
	$(CPI) bin\ExMODIst.exe ind\ExMODIst.exe
	$(CPI) DCHookTest\DCHookTest.exe ind\DKPu.exe
	$(CPI) DKPU.TXT ind\DKPu.txt

buildis:
	cd ind
	start dkpu.iss

zip_src:
	zip -r src . -i *.h *.c *.cpp *.cs *.dfm *.bpr *.bpf *.bpg *.rc *.ico *.bmp *.png *.txr *.ctt *.bat *.ind *.iss *.tpl *.manifest *.txt *.sln *.vcproj *.vcxproj *.user
	zip -r src-amodi \src\amodi -i *.h *.c *.cpp *.cs *.dfm *.bpr *.bpf *.bpg *.rc *.ico *.bmp *.png *.txr *.ctt *.bat *.ind *.iss *.tpl *.manifest *.txt *.sln *.vcproj *.vcxproj *.user

deploy:
	mktplu
	mku
