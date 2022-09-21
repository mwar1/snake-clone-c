#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct Button {
	int x;
	int y;
	int width;
	int height;
	char text[20];
	int bgColour[4];
	int outlineColour[4];
	int textColour[4];
	int fontSize;
	int returnValue;
} Button;

typedef struct Text {
	int x;
	int y;
	bool centred;
	char text[20];
	int fontSize;
	int colour[4];
} Text;

int getReturnValue(Button);
bool isClicked(Button, int, int);
void drawButton(SDL_Renderer*, Button);
void drawText(SDL_Renderer*, Text);