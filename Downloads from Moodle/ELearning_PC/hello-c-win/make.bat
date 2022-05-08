cls

C:\\msys64\\mingw64\\bin\\g++ -c -o hello.o hello.cpp -Wall -mwindows -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I../../fltk -I../../fltk/png -I../../fltk/jpeg -I.

C:\\msys64\\mingw64\\bin\\g++ -o hello.exe hello.o -mwindows ../../fltk/lib/libfltk.a -lole32 -luuid -lcomctl32 -lws2_32 -lz ../../fltk/lib/libfltk_images.a ../../fltk/lib/libfltk_jpeg.a ../../fltk/lib/libfltk_png.a

hello.exe
