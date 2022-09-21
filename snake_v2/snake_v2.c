#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "widget.h"

#define SCREENWIDTH 1003
#define MAXLENGTH 512

SDL_Color BLACK = {0, 0, 0};
int numSquares, squareWidth, numApples, snakeSpeed, offset, snakeLength, apples[5][2];
char numSquaresText[10], numApplesText[10], snakeSpeedText[10];
bool empty, turned, localEmpty;

typedef struct {
	int x;
	int y;
	int direction[2];
} SnakePart;
SnakePart *snake[MAXLENGTH];

bool isAppleFree(int apple[], int i) {
	// Check if an apple is not in a square already taken
	// by a snake part or another apple

	for (int j=0; j<snakeLength; j++) {
		int temp[2] = {snake[j]->x, snake[j]->y};
		empty = !(memcmp(&apple, &temp, sizeof(temp)));
		if (!empty) return false;
	}
	if (empty) {
		for (int k=0; k<numApples; k++) {
			int temp[2] = {apples[k][0], apples[k][1]};
			if (i != k) empty = !(memcmp(&apple, &temp, sizeof(temp)));
			if (!empty) return false;
		}
	}
	return true;
}

int startScreen(SDL_Renderer *renderer) {
	bool close = false;
	int mouseX, mouseY;

	Button buttons[7] = {{SCREENWIDTH-275-10, SCREENWIDTH-110-10, 275, 110, "Play", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 95, -1},

					 	 {SCREENWIDTH*0.1, 265, 70, 70, "<-", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 50, 1},
					 	 {SCREENWIDTH*0.9-70, 265, 70, 70, "->", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 50, 2},

					 	 {SCREENWIDTH*0.1, 515, 70, 70, "<-", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 50, 3},
					 	 {SCREENWIDTH*0.9-70, 515, 70, 70, "->", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 50, 4},

					 	 {SCREENWIDTH*0.1, 765, 70, 70, "<-", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 50, 5},
					 	 {SCREENWIDTH*0.9-70, 765, 70, 70, "->", {0, 200, 0, 255},
					 	 {0, 155, 0, 255}, {0, 0, 0, 255}, 50, 6}};

	snprintf(snakeSpeedText, 10, "%i", snakeSpeed);
	snprintf(numSquaresText, 10, "%i", numSquares);
	snprintf(numApplesText, 10, "%i", numApples);

	Text texts[7] = {{SCREENWIDTH/2, 75, true, "Settings", 100, {0, 0, 0, 255}},
					 {10, 150, false, "Snake Speed", 50, {25, 25, 25, 255}},
					 {10, 400, false, "No. Squares", 50, {25, 25, 25, 255}},
					 {10, 650, false, "No. Apples", 50, {25, 25, 25, 255}},
					 {SCREENWIDTH/2, 300, true, "snakeSpeedText", 50, {25, 25, 25, 255}},
					 {SCREENWIDTH/2, 550, true, "numSquaresText", 50, {25, 25, 25, 255}},
					 {SCREENWIDTH/2, 800, true, "numApplesText", 50, {25, 25, 25, 255}}};
	strcpy(texts[4].text, snakeSpeedText);
	strcpy(texts[5].text, numSquaresText);
	strcpy(texts[6].text, numApplesText);

	while (!close) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				return 0;
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				SDL_GetMouseState(&mouseX, &mouseY);
				
				for (int i=0; i<sizeof(buttons)/sizeof(Button); i++) {
					if (isClicked(buttons[i], mouseX, mouseY)) {
						int output = getReturnValue(buttons[i]);
						if (output == -1) return 1;

						if (output == 1 && snakeSpeed > 1) {
							snakeSpeed--;
						} else if (output == 2 && snakeSpeed < 15) {
							snakeSpeed++;
						} else if (output == 3 && numSquares > 10) {
							numSquares--;
						} else if (output == 4 && numSquares < 25) {
							numSquares++;
						} else if (output == 5 && numApples > 1) {
							numApples--;
						} else if (output == 6 && numApples < 5) {
							numApples++;
						}
						squareWidth = SCREENWIDTH / numSquares;
						offset = (SCREENWIDTH - (squareWidth * numSquares)) / 2;

						snprintf(snakeSpeedText, 10, "%i", snakeSpeed);
						snprintf(numSquaresText, 10, "%i", numSquares);
						snprintf(numApplesText, 10, "%i", numApples);

						strcpy(texts[4].text, snakeSpeedText);
						strcpy(texts[5].text, numSquaresText);
						strcpy(texts[6].text, numApplesText);
					}
				}
			}
		}

		// Fill the screen light-green
		SDL_SetRenderDrawColor(renderer, 0, 165, 0, 255);
		SDL_RenderClear(renderer);

		// Draw text
		for (int i=0; i<sizeof(texts)/sizeof(Text); i++) {
			drawText(renderer, texts[i]);
		}

		// Draw buttons
		for (int i=0; i<sizeof(buttons)/sizeof(Button); i++) {
			drawButton(renderer, buttons[i]);
		}

		// Actually draw the screen and tick the clock
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/60);
	}
}

int mainGame(SDL_Renderer *renderer) {
	SDL_Delay(200);

	TTF_Font *font = TTF_OpenFont("../Fonts/font.otf", 80);
	TTF_Font *smallFont = TTF_OpenFont("../Fonts/font.otf", 42);

	int snakeLength = 3;

	SDL_Texture *deathText, *scoreText, *enterText, *currentScoreText = NULL;
	SDL_Rect deathRect, scoreRect, enterRect;

	Button settingButton = {SCREENWIDTH/2-175, SCREENWIDTH*0.75, 350, 75, "Settings", {50, 50, 50, 0}, {0, 0, 0, 255}, {0, 0, 0, 255}, 70, 1};

	for (int i=0; i<snakeLength; i++) {
		SnakePart *temp = malloc(sizeof(SnakePart));
		temp->x = numSquares/2 - 2 - i;
		temp->y = numSquares / 2;
		temp->direction[0] = 1;
		temp->direction[1] = 0;
		memcpy(&snake[i], &temp, sizeof(SnakePart));
	}

	// Generate the apples
	for (int i=0; i<numApples; i++) {
		localEmpty = false;
		while (!localEmpty) {
			apples[i][0] = rand() % numSquares;
			apples[i][1] = rand() % numSquares;
			localEmpty = isAppleFree(apples[i], i);
		}
	}

	int counter = 0;
	int score = 0;
	bool dead, textGenned, close = false;
	while (!close) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				close = true;
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				if (isClicked(settingButton, mouseX, mouseY)) return 1;
			}
		}

		// Game Logic
		const Uint8 *keysPressed = SDL_GetKeyboardState(NULL);
		if ((keysPressed[SDL_SCANCODE_W] || keysPressed[SDL_SCANCODE_UP]) && snake[0]->direction[1] == 0 && !turned) {
			snake[0]->direction[0] = 0;
			snake[0]->direction[1] = -1;
			turned = true;
		} else if ((keysPressed[SDL_SCANCODE_A]  || keysPressed[SDL_SCANCODE_LEFT]) && snake[0]->direction[0] == 0 && !turned) {
			snake[0]->direction[0] = -1;
			snake[0]->direction[1] = 0;
			turned = true;
		} else if ((keysPressed[SDL_SCANCODE_S]  || keysPressed[SDL_SCANCODE_DOWN]) && snake[0]->direction[1] == 0 && !turned) {
			snake[0]->direction[0] = 0;
			snake[0]->direction[1] = 1;
			turned = true;
		} else if ((keysPressed[SDL_SCANCODE_D]  || keysPressed[SDL_SCANCODE_RIGHT]) && snake[0]->direction[0] == 0 && !turned) {
			snake[0]->direction[0] = 1;
			snake[0]->direction[1] = 0;
			turned = true;
		} else if (keysPressed[SDL_SCANCODE_RETURN] && dead) {
			// Reset the game
			time_t t;
			srand((unsigned) time(&t));

			snake[0]->direction[0] = 1;
			snake[0]->direction[1] = 0;
			snakeLength = 3;
			memset(snake, 0, MAXLENGTH*sizeof(*snake));

			SDL_Texture *deathText, *scoreText, *enterText = NULL;
			SDL_Rect deathRect, scoreRect, enterRect;

			for (int i=0; i<snakeLength; i++) {
				SnakePart *temp = malloc(sizeof(SnakePart));
				temp->x = 8-i;
				temp->y = 7;
				temp->direction[0] = 1;
				temp->direction[1] = 0;
				memcpy(&snake[i], &temp, sizeof(SnakePart));
			}

			for (int i=0; i<3; i++) {
				apples[i][0] = -1;
				apples[i][1] = -1;
			}

			for (int i=0; i<numApples; i++) {
				localEmpty = false;
				while (!localEmpty) {
					apples[i][0] = rand() % numSquares;
					apples[i][1] = rand() % numSquares;
					localEmpty = isAppleFree(apples[i], i);
				}
			}

			counter = 0;
			score = 0;
			dead = false;
			textGenned = false;
		}

		// Update the snake position
		if (!dead && counter % (18 - snakeSpeed) == 0) {
			turned = false;
			for (int i=snakeLength-1; i>=0; i--) {
				snake[i]->x += snake[i]->direction[0];
				snake[i]->y += snake[i]->direction[1];

				if (i != 0) {
					snake[i]->direction[0] = snake[i-1]->direction[0];
					snake[i]->direction[1] = snake[i-1]->direction[1];
				}
			}
			counter = 0;
		}
		counter++;

		// Check if an apple has been reached
		for (int i=0; i<numApples; i++) {
			if (snake[0]->x == apples[i][0] && snake[0]->y == apples[i][1]) {
				snake[snakeLength] = malloc(sizeof(SnakePart));
				snake[snakeLength]->x = snake[snakeLength-1]->x - snake[snakeLength-1]->direction[0];
				snake[snakeLength]->y = snake[snakeLength-1]->y - snake[snakeLength-1]->direction[1];
				snake[snakeLength]->direction[0] = snake[snakeLength-1]->direction[0];
				snake[snakeLength]->direction[1] = snake[snakeLength-1]->direction[1];
				snakeLength++;
				score++;

				localEmpty = false;
				while (!localEmpty) {
					apples[i][0] = rand() % numSquares;
					apples[i][1] = rand() % numSquares;
					localEmpty = isAppleFree(apples[i], i);
				}
			}
		}

		// Check if the snake has collided with itself or a wall
		for (int i=1; i<snakeLength-1; i++) {
			if (snake[i]->x == snake[0]->x && snake[i]->y == snake[0]->y) {
				dead = true;
			}
		}
		if (!dead) {
			dead = (snake[0]->x < 0 || snake[0]->x >= numSquares ||
					snake[0]->y < 0 || snake[0]->y >= numSquares);
		}

		// Clear the screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Draw the light green background
		SDL_SetRenderDrawColor(renderer, 0, 165, 0, 255);
		SDL_Rect background = {offset, offset, SCREENWIDTH-offset*2, SCREENWIDTH-offset*2};
		SDL_RenderFillRect(renderer, &background);

		// Draw the checkerboard on top
		SDL_SetRenderDrawColor(renderer, 0, 155, 0, 255);
		for (int i=0; i<numSquares; i++) {
			for (int j=0; j<numSquares; j++) {
				if ((i*19+j) % 2 == 0) {
					SDL_Rect checker = {offset+i*(squareWidth), offset+j*(squareWidth), squareWidth, squareWidth};
					SDL_RenderFillRect(renderer, &checker);
				}
			}
		}

		// Draw the snake
		for (int i=0; i<snakeLength; i++) {
			SDL_Rect snakeRect = {offset+snake[i]->x*squareWidth, offset+snake[i]->y*squareWidth, squareWidth, squareWidth};
			if (i % 2 == 0) {
				SDL_SetRenderDrawColor(renderer, 255, 140, 235, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 255, 170, 240, 255);
			}
			SDL_RenderFillRect(renderer, &snakeRect);
		}

		// Draw the apples
		for (int i=0; i<numApples; i++) {
			SDL_Rect appleRect = {offset+apples[i][0]*squareWidth, offset+apples[i][1]*squareWidth, squareWidth, squareWidth};

			SDL_SetRenderDrawColor(renderer, 255, 15, 15, 255);
			SDL_RenderFillRect(renderer, &appleRect);
		}

		// Draw the current score
		char currentScore[12];
		snprintf(currentScore, 12, "Score : %i", score);

		SDL_Surface *currentScoreSurf = TTF_RenderText_Blended(smallFont, currentScore, BLACK);
		currentScoreText = SDL_CreateTextureFromSurface(renderer, currentScoreSurf);
		SDL_FreeSurface(currentScoreSurf);

		int w, h;
		TTF_SizeText(smallFont, currentScore, &w, &h);
		SDL_Rect currentScoreRect = {10, 10, w, h};
		SDL_RenderCopy(renderer, currentScoreText, NULL, &currentScoreRect);

		char buf[12];
		if (dead) {
			if (!textGenned) {
				SDL_Surface *deathSurf = TTF_RenderText_Blended(font, "GAME OVER", BLACK);
				deathText = SDL_CreateTextureFromSurface(renderer, deathSurf);
				SDL_FreeSurface(deathSurf);

				snprintf(buf, 12, "Score : %i", score);
				SDL_Surface *scoreSurf = TTF_RenderText_Blended(font, buf, BLACK);
				scoreText = SDL_CreateTextureFromSurface(renderer, scoreSurf);
				SDL_FreeSurface(scoreSurf);

				SDL_Surface *enterSurf = TTF_RenderText_Blended(smallFont, "Press Enter to play again", BLACK);
				enterText = SDL_CreateTextureFromSurface(renderer, enterSurf);
				SDL_FreeSurface(enterSurf);

				textGenned = true;
			} else {
				int w, h;
				TTF_SizeText(font, "GAME OVER", &w, &h);
				SDL_Rect deathRect = {SCREENWIDTH/2 - w/2, SCREENWIDTH*0.25, w, h};

				TTF_SizeText(font, buf, &w, &h);
				SDL_Rect scoreRect = {SCREENWIDTH/2 - w/2, SCREENWIDTH*0.35, w, h};

				TTF_SizeText(smallFont, "Press Enter to play again", &w, &h);
				SDL_Rect enterRect = {SCREENWIDTH/2 - w/2, SCREENWIDTH*0.6, w, h};

				SDL_RenderCopy(renderer, deathText, NULL, &deathRect) != 0;
				SDL_RenderCopy(renderer, scoreText, NULL, &scoreRect) != 0;
				SDL_RenderCopy(renderer, enterText, NULL, &enterRect) != 0;

				drawButton(renderer, settingButton);
			}
		}

		// Actually draw the screen and tick the clock
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/45);
	}
	SDL_DestroyTexture(currentScoreText);
	SDL_DestroyTexture(deathText);
	SDL_DestroyTexture(scoreText);
	SDL_DestroyTexture(enterText);
	return 0;
}

int main(int argc, char *argv[]) {
	time_t t;
	srand((unsigned) time(&t));

	numSquares = 17;
	squareWidth = SCREENWIDTH / numSquares;
	numApples = 1;
	snakeSpeed = 10;
	turned = false;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}
	TTF_Init();

	SDL_Window *window = SDL_CreateWindow("Snake",
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  SCREENWIDTH, SCREENWIDTH, 0);

	Uint32 renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, renderFlags);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	while (startScreen(renderer) && mainGame(renderer));

	// Clean up resources before exiting
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();
	return 0;
}