copy ..\DCHook\VS2015\Release\DCHook.dll DKPUHK.DLL
copy ..\DCHook\VS2015\x64\Release\DCHook.dll DKPUHK64.DLL
copy ..\atsocr\atsocr\bin\Release\atsocr.exe .
copy ..\atsocr\atsocr\bin\Release\atsocr.exe.config .
copy ..\atsocr\atsocr\bin\Release\Tesseract.dll .
mkdir x64
mkdir x86
copy ..\atsocr\atsocr\bin\Release\x86\leptonica-1.82.0.dll .\x86\
copy ..\atsocr\atsocr\bin\Release\x86\tesseract50.dll .\x86\
copy ..\atsocr\atsocr\bin\Release\x64\leptonica-1.82.0.dll .\x64\
copy ..\atsocr\atsocr\bin\Release\x64\tesseract50.dll .\x64\

rem copy ..\DCHook64\dchk64\x64\Release\dchk64.exe DKPUHK64.exe
