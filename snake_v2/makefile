all : build

build: snake_v2.c widget.c
	gcc snake_v2.c widget.c \
	-Wfatal-errors \
	-std=c99 \
	-I"C:\libsdl\include" \
	-L"C:\libsdl\lib" \
	-lmingw32 \
	-lSDL2main \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-o "snake_v2.exe"

clean:
	del /f *.o snake_v2.exe