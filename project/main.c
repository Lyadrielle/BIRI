/**
 * @file	main.c
 *       	Entry point of the program. Define and execute all basic SDL functions,
 *       	 such as initialise, set, reshape... the game window.
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date	2017-04-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "headers.h"

/*/////////////////////////////////////////
 //			GLOBAL VARIABLES DEF		//
/////////////////////////////////////////*/

char *playersNames[4];
GLuint texturesBuffer[TEXTURE_NB];
Color3f themeColor;

/*/////////////////////////////////////////
 //			MAIN SDL FUNCTIONS			//
/////////////////////////////////////////*/

/**
 * Reajust the axis of the window, x and y start at 0 at the top left corner
 * @param	int	screenWidth	new screenWidth calculated with the config file gridWith
 */
void reshape() {
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
}

/**
 * Set the video mode for an SDL_OPENGL window.
 * @param int screenWidth new screenWidth calculated with the config file gridWith
 */
void setVideoMode() {
	if(NULL == SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL)) {
		fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
		exit(EXIT_FAILURE);
	}
	reshape();
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapBuffers();
}

void instanciatePlayerNames(int argc, char** argv) {
	if(argc < 2) {
		printf("ERREUR : Config file manquant\n");
		return;
	} else if (argc == 2) {
		playersNames[0] = "Player 1";
		playersNames[1] = "Player 2";
		playersNames[2] = "Player 3";
		playersNames[3] = "Player 4";
	} else if (argc == 3) {
		playersNames[0] = argv[2];
		playersNames[1] = "Player 2";
		playersNames[2] = "Player 3";
		playersNames[3] = "Player 4";
	} else if (argc == 4) {
		playersNames[0] = argv[2];
		playersNames[1] = argv[3];
		playersNames[2] = "Player 3";
		playersNames[3] = "Player 4";
	} else if (argc == 5) {
		playersNames[0] = argv[2];
		playersNames[1] = argv[3];
		playersNames[2] = argv[4];
		playersNames[3] = "Player 4";
	} else if (argc == 6){
		playersNames[0] = argv[2];
		playersNames[1] = argv[3];
		playersNames[2] = argv[4];
		playersNames[3] = argv[5];
	}
}

/*/////////////////////////////////////////
 //			MAIN FUNCTION START			//
/////////////////////////////////////////*/

/**
 * Main function
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int main (int argc, char** argv) {
	Uint8 * keyState = SDL_GetKeyState(NULL);
	int gridWidth = 0, gridHeight = 0;
	int *brickTypes;
	bool gladOS = false;
	glutInit(&argc, argv);
	initColor3f(&themeColor, 255, 139, 0);
	instanciatePlayerNames(argc, argv);

	brickTypes = readConfigFile(argv[1], &gridWidth, &gridHeight);

	if (-1 == SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
		return EXIT_FAILURE;
	}

	/* set the openGL SDL context */
	setVideoMode(SCREEN_WIDTH);
	/* Set a title to the window */
	SDL_WM_SetCaption("KassPong", NULL);

	/*/////////////////////////////////////////
	 //				INITIALISATONS			//
	/////////////////////////////////////////*/
	Button *menu = malloc(NB_BUTTON_MAIN_MENU * sizeof(Button));
	initMenu(menu);
	GridBrick grid = initGrid(gridWidth, gridHeight, brickTypes);
	initBrickCoordinates(grid, gridWidth, gridHeight);
	glGenTextures(TEXTURE_NB, texturesBuffer);
	loadTextures("img/THEME1/");

	/*/////////////////////////////////////////
	 //				INFINITE LOOP			//
	/////////////////////////////////////////*/
	int tmp, i, j, loop = true, nbPlayers = 0, nbBalls = 0;
	int gameStep = INITIALISATON;
	while (loop) {
		/*/////////////////////////////////////////
		 //				EVENT MANAGER			//
		/////////////////////////////////////////*/
		SDL_Event trigger;
		/*----------( STOP CONDITION )---------- */
		while(SDL_PollEvent(&trigger)) {
			if (trigger.type == SDL_QUIT) {
				loop = 0;
				break;
			}
			switch (gameStep) {
				case INITIALISATON :
					tmp = handleButton(&menu[0], trigger, &gameStep);
					if (tmp == 1) gladOS = true;
					if (tmp && tmp <= 4)	nbPlayers = tmp + 1;
					tmp = handleButton(&menu[1], trigger, &gameStep);
					if (tmp && tmp <= 4)	nbPlayers = tmp;
					tmp = handleButton(&menu[2], trigger, &gameStep);
					if (tmp && tmp <= 4)	nbPlayers = tmp;
					if (tmp == 4 && gridWidth > 7)  {
						gridWidth = 7;
						initBrickCoordinates(grid, gridWidth, gridHeight);
					}
					handleButton(&menu[3], trigger, &gameStep);
					handleButton(&menu[4], trigger, &gameStep);
					nbBalls = nbPlayers;
					break;
				case PLAYTIME :
					break;
				case SCOREBOARD :
					break;
				case QUIT_PROGRAM :
					loop = false;
					break;
				case PAUSE :
					break;
				default :
					break;
			}
		}


		/*/////////////////////////////////////////
		 //				GAME MANAGER			//
		/////////////////////////////////////////*/
		glClear(GL_COLOR_BUFFER_BIT);
		/* ----------( INITIALISATION FASE )---------- */
		if (gameStep == INITIALISATON) {
			drawBackground(13);
			drawMenu(menu);
		}
		/* -------------( PLAYTIME FASE )------------ */
		if (gameStep == PLAYTIME) {
			drawBackground(4);
			for (i = 0; i < nbBalls; ++i) {
				collisionBallScreen(&balls[i], nbPlayers);
				for (j = 0; j < nbPlayers; ++j) {
					collisionBarBall(&(players[j].bar), &balls[i]);
				}
				collisionBallGrid(grid, &balls[i], gridWidth, gridHeight);
			}

			for (i = 0; i < nbBalls; ++i) {
				if (!balls[i].respawnTimer) {
					moveBall(&balls[i]);
					drawBall(balls[i]);
				} else {
					--(balls[i].respawnTimer);
				}
				if (!balls[i].bonusTimer) {
					balls[i].speed.x = balls[i].speed.x < 0 ? - NORMAL : NORMAL;
					balls[i].speed.y = balls[i].speed.y < 0 ? - NORMAL : NORMAL;
				} else {
					--(balls[i].bonusTimer);
				}
			}

			for (j = 0; j < nbPlayers; ++j) {
				drawHUD(&players[j], nbPlayers);
				drawBar(players[j].bar);
			}
			drawGrid(grid, gridWidth, gridHeight);

			for (i = 0; i < nbPlayers; ++i) {
				if (players[i].life <= 0) {
					++gameStep;
				}
			}
		}

		if (gameStep == SCOREBOARD) {
			drawBackground(14);
			printVictoryScreen(players, nbPlayers, gladOS);
		}

		if (gameStep == PAUSE) {
			drawBackground(14);
		}

		/* -------------( SCOREBOARD FASE )------------ */
		SDL_GL_SwapBuffers();

		/*/////////////////////////////////////////
		 //			KEYBOARD MANAGER			//
		/////////////////////////////////////////*/
		if (keyState[SDLK_p] && trigger.type == SDL_KEYDOWN && gameStep == PLAYTIME) {
			gameStep = PAUSE;
		} else if (keyState[SDLK_p] && trigger.type == SDL_KEYDOWN && gameStep == PAUSE) {
			gameStep = PLAYTIME;
		}

		if (gameStep == PLAYTIME) {
			keyState = SDL_GetKeyState(NULL);
			if (keyState[SDLK_a]) moveBar(&(players[0].bar), LEFT);
			if (keyState[SDLK_z]) moveBar(&(players[0].bar), RIGHT);

			/* IA */
			if (!gladOS) {
				if (keyState[SDLK_LEFT]) moveBar(&(players[1].bar), LEFT);
				if (keyState[SDLK_RIGHT]) moveBar(&(players[1].bar), RIGHT);
			} else {
				players[1].name = "GladOS";
				handleGladOS(&(players[1].bar), balls, nbPlayers);
			}

			/* 4 PLAYERS */
			if (nbPlayers > 2) {
				if (keyState[SDLK_u]) moveBar(&(players[2].bar), TOP);
				if (keyState[SDLK_n]) moveBar(&(players[2].bar), BOTTOM);

				if (keyState[SDLK_KP9]) moveBar(&(players[3].bar), TOP);
				if (keyState[SDLK_KP3]) moveBar(&(players[3].bar), BOTTOM);
			}

			SDL_Delay(5);
		}
	}

	if (menu != NULL) {
		free(menu);
	}
	if (grid != NULL) {
		for (i = 0; i < gridHeight; ++i) {
			if (grid[i] != NULL) {
				free(grid[i]);
			}
		}
		free(grid);
	}
	if (brickTypes != NULL) {
		free(brickTypes);
	}
	if (players != NULL) {
		free(players);
	}
	if (balls != NULL) {
		free(balls);
	}
	glDeleteTextures(TEXTURE_NB, texturesBuffer);
	SDL_Quit();
	return EXIT_SUCCESS;
}
