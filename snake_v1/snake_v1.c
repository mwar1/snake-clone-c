#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SCREENWIDTH 1003
#define SQUAREWIDTH 59
#define NUMSQUARES SCREENWIDTH/SQUAREWIDTH
#define MAXLENGTH 512

bool close = false;
SDL_Color black = {0, 0, 0};

typedef struct {
	int x;
	int y;
	int direction[2];
} SnakePart;

int main(int argc, char *argv[]) {
	time_t t;
	srand((unsigned) time(&t));

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}
	TTF_Init();
	TTF_Font *font = TTF_OpenFont("../Fonts/font.otf", 80);
	TTF_Font *smallFont = TTF_OpenFont("../Fonts/font.otf", 42);

	SDL_Window *window = SDL_CreateWindow("Snake",
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  SCREENWIDTH, SCREENWIDTH, 0);
	Uint32 renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, renderFlags);

	int snakeLength = 3;
	SnakePart *snake[MAXLENGTH];

	SDL_Texture *deathText, *scoreText, *enterText, *currentScoreText = NULL;
	SDL_Rect deathRect, scoreRect, enterRect;

	for (int i=0; i<snakeLength; i++) {
		SnakePart *temp = malloc(sizeof(SnakePart));
		temp->x = 8-i;
		temp->y = 7;
		temp->direction[0] = 1;
		temp->direction[1] = 0;
		memcpy(&snake[i], &temp, sizeof(SnakePart));
	}
	int apple[2] = {14, 7};

	int counter = 0;
	int score = 0;
	bool dead = false;
	bool textGenned = false;
	while (!close) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				close = true;
			}
		}

		// Game Logic
		const Uint8 *keysPressed = SDL_GetKeyboardState(NULL);
		if (keysPressed[SDL_SCANCODE_W] && snake[0]->direction[1] == 0) {
			snake[0]->direction[0] = 0;
			snake[0]->direction[1] = -1;
		} else if (keysPressed[SDL_SCANCODE_A] && snake[0]->direction[0] == 0) {
			snake[0]->direction[0] = -1;
			snake[0]->direction[1] = 0;
		} else if (keysPressed[SDL_SCANCODE_S] && snake[0]->direction[1] == 0) {
			snake[0]->direction[0] = 0;
			snake[0]->direction[1] = 1;
		} else if (keysPressed[SDL_SCANCODE_D] && snake[0]->direction[0] == 0) {
			snake[0]->direction[0] = 1;
			snake[0]->direction[1] = 0;
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
			apple[0] = 14;
			apple[1] = 7;

			counter = 0;
			score = 0;
			dead = false;
			textGenned = false;
		}

		// Update the snake position
		if (!dead && counter % 10 == 0) {
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

		// Check if apple has been reached
		if (snake[0]->x == apple[0] && snake[0]->y == apple[1]) {
			snake[snakeLength] = malloc(sizeof(SnakePart));
			snake[snakeLength]->x = snake[snakeLength-1]->x - snake[snakeLength-1]->direction[0];
			snake[snakeLength]->y = snake[snakeLength-1]->y - snake[snakeLength-1]->direction[1];
			snake[snakeLength]->direction[0] = snake[snakeLength-1]->direction[0];
			snake[snakeLength]->direction[1] = snake[snakeLength-1]->direction[1];
			snakeLength++;
			score++;

			bool empty = false;
			while (!empty) {
				apple[0] = rand() % NUMSQUARES;
				apple[1] = rand() % NUMSQUARES;
				empty = true;

				for (int i=0; i<snakeLength; i++) {
					if (snake[i]->x == apple[0] && snake[i]->y == apple[1]) empty = false;
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
			dead = (snake[0]->x < 0 || snake[0]->x >= NUMSQUARES ||
					snake[0]->y < 0 || snake[0]->y >= NUMSQUARES);
		}

		// Fill the screen light-green
		SDL_SetRenderDrawColor(renderer, 0, 165, 0, 255);
		SDL_RenderClear(renderer);

		// Draw the checkerboard on top
		SDL_SetRenderDrawColor(renderer, 0, 155, 0, 255);
		for (int i=0; i<NUMSQUARES; i++) {
			for (int j=0; j<NUMSQUARES; j++) {
				if ((i*19+j) % 2 == 0) {
					SDL_Rect checker = {i*(SQUAREWIDTH), j*(SQUAREWIDTH), SQUAREWIDTH, SQUAREWIDTH};
					SDL_RenderFillRect(renderer, &checker);
				}
			}
		}

		// Draw the snake
		for (int i=0; i<snakeLength; i++) {
			SDL_Rect snakeRect = {snake[i]->x*SQUAREWIDTH, snake[i]->y*SQUAREWIDTH, SQUAREWIDTH, SQUAREWIDTH};
			if (i % 2 == 0) {
				SDL_SetRenderDrawColor(renderer, 255, 140, 235, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 255, 170, 240, 255);
			}
			SDL_RenderFillRect(renderer, &snakeRect);
		}

		// Draw the apple
		SDL_Rect appleRect = {apple[0]*SQUAREWIDTH, apple[1]*SQUAREWIDTH, SQUAREWIDTH, SQUAREWIDTH};

		SDL_SetRenderDrawColor(renderer, 255, 15, 15, 255);
		SDL_RenderFillRect(renderer, &appleRect);

		// Draw the current score
		char currentScore[11];
		snprintf(currentScore, 11, "Score : %i", score);

		SDL_Surface *currentScoreSurf = TTF_RenderText_Blended(smallFont, currentScore, black);
		currentScoreText = SDL_CreateTextureFromSurface(renderer, currentScoreSurf);
		SDL_FreeSurface(currentScoreSurf);

		int w, h;
		TTF_SizeText(smallFont, currentScore, &w, &h);
		SDL_Rect currentScoreRect = {5, 5, w, h};
		SDL_RenderCopy(renderer, currentScoreText, NULL, &currentScoreRect);

		char buf[11];
		if (dead) {
			if (!textGenned) {
				SDL_Surface *deathSurf = TTF_RenderText_Blended(font, "GAME OVER", black);
				deathText = SDL_CreateTextureFromSurface(renderer, deathSurf);
				SDL_FreeSurface(deathSurf);

				snprintf(buf, 11, "Score : %i", score);
				SDL_Surface *scoreSurf = TTF_RenderText_Blended(font, buf, black);
				scoreText = SDL_CreateTextureFromSurface(renderer, scoreSurf);
				SDL_FreeSurface(scoreSurf);

				SDL_Surface *enterSurf = TTF_RenderText_Blended(smallFont, "Press Enter to play again", black);
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

				if (SDL_RenderCopy(renderer, deathText, NULL, &deathRect) != 0) printf("Error drawing deathText : %s\n", SDL_GetError());
				if (SDL_RenderCopy(renderer, scoreText, NULL, &scoreRect) != 0) printf("Error drawing scoreText : %s\n", SDL_GetError());
				if (SDL_RenderCopy(renderer, enterText, NULL, &enterRect) != 0) printf("Error drawing scoreText : %s\n", SDL_GetError());
			}
		}

		// Actually draw the screen and tick the clock
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/60);
	}

	// Clean up resources before exiting
	SDL_DestroyTexture(currentScoreText);
	SDL_DestroyTexture(deathText);
	SDL_DestroyTexture(scoreText);
	SDL_DestroyTexture(enterText);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}