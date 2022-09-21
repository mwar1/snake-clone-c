#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "widget.h"

int getReturnValue(Button button) {
	return button.returnValue;
}

bool isClicked(Button button, int mouseX, int mouseY) {
	SDL_Point mouse = {mouseX, mouseY};
	SDL_Rect buttonRect = {button.x, button.y, button.width, button.height};

	return SDL_PointInRect(&mouse, &buttonRect);
}

void drawButton(SDL_Renderer *renderer, Button button) {
	TTF_Font *font = TTF_OpenFont("../fonts/font.otf", button.fontSize);
	SDL_Rect buttonRect = {button.x, button.y, button.width, button.height};

	// Draw background
	SDL_SetRenderDrawColor(renderer, button.bgColour[0], button.bgColour[1], button.bgColour[2], button.bgColour[3]);
	SDL_RenderFillRect(renderer, &buttonRect);

	// Draw outline
	SDL_SetRenderDrawColor(renderer, button.outlineColour[0], button.outlineColour[1], button.outlineColour[2], button.outlineColour[3]);
	SDL_RenderDrawRect(renderer, &buttonRect);

	// Draw text
	SDL_Color textColour = {button.textColour[0], button.textColour[1], button.textColour[2], button.textColour[3]};
	SDL_Surface *textSurf = TTF_RenderText_Blended(font, button.text, textColour);
	SDL_Texture *textText = SDL_CreateTextureFromSurface(renderer, textSurf);
	SDL_FreeSurface(textSurf);

	int w, h;
	TTF_SizeText(font, button.text, &w, &h);
	SDL_Rect textRect = {button.x + (button.width/2 - w/2), button.y + (button.height/2 - h/2), w, h};
	SDL_RenderCopy(renderer, textText, NULL, &textRect);
	SDL_DestroyTexture(textText);
}

void drawText(SDL_Renderer *renderer, Text text) {
	TTF_Font *font = TTF_OpenFont("../fonts/font.otf", text.fontSize);
	SDL_Color textColour = {text.colour[0], text.colour[1], text.colour[2], text.colour[3]};
	SDL_Rect textRect;

	int w, h;
	TTF_SizeText(font, text.text, &w, &h);
	if (text.centred) {
		textRect.x = text.x-w/2;
		textRect.y = text.y-h/2;
	} else {
		textRect.x = text.x;
		textRect.y = text.y;
	}
	textRect.w = w;
	textRect.h = h;

	SDL_Surface *textSurf = TTF_RenderText_Blended(font, text.text, textColour);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurf);
	SDL_FreeSurface(textSurf);

	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_DestroyTexture(textTexture);
}