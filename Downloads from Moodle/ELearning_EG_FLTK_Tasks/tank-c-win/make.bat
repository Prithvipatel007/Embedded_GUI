cls
setlocal

:: if not started from vscode find project name
if "%1"=="" ( for %%i in (.) do set vscodedirname=%%~nxi ) else ( set vscodedirname=%1 )

:: source name
if "%1"=="" ( set vscodesrcname=%vscodedirname%.cpp ) else ( set vscodesrcname=%1.cpp )

:: remove space before .cpp
set vscodesrcname=%vscodesrcname: =%

:: object name
if "%1"=="" ( set vscodeobjname=%vscodedirname%.o ) else ( set vscodeobjname=%1.o )

:: remove space before .o
set vscodeobjname=%vscodeobjname: =%

:: app name
if "%1"=="" ( set vscodeappname=%vscodedirname%.exe ) else ( set vscodeappname=%1.exe )

:: remove space before .exe
set vscodeappname=%vscodeappname: =%

C:\\msys64\\mingw64\\bin\\g++ -c -o %vscodeobjname% %vscodesrcname% -Wall -mwindows -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I../../fltk -I../../fltk/png -I../../fltk/jpeg -I.

C:\\msys64\\mingw64\\bin\\g++ -o %vscodeappname% %vscodeobjname% -mwindows ../../fltk/lib/libfltk.a -lole32 -luuid -lcomctl32 -lws2_32 -lz ../../fltk/lib/libfltk_images.a ../../fltk/lib/libfltk_jpeg.a ../../fltk/lib/libfltk_png.a

%vscodeappname%
