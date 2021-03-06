/**
 * @file		display.c
 *       		display functions library. Centralise all basic display functions,
 * 			    basic shapes non dependent of the gameplay.
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date		2017-04-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <SDL/SDL_image.h>

#include "headers.h"

/*/////////////////////////////////////////
 //				BASIC DRAWING FUNCTIONS				//
/////////////////////////////////////////*/

/**
 * Draw a centered bar based on the player's id.
 * @param	Bar	bar	the current bar to draw
 */
void drawBar(Bar bar) {
	int sizeX = bar.width / 2;
	int sizeY = BAR_HEIGHT / 2;

	glColor3f(bar.color.r, bar.color.g, bar.color.b);

	if (!bar.orientationHorizontal) {
		sizeX = BAR_HEIGHT / 2;
		sizeY = bar.width / 2;
	}

	glBegin(GL_POLYGON);
		glVertex2f((bar.center.x - sizeX), (bar.center.y - sizeY));
		glVertex2f((bar.center.x + sizeX), (bar.center.y - sizeY));
		glVertex2f((bar.center.x + sizeX), (bar.center.y + sizeY));
		glVertex2f((bar.center.x - sizeX), (bar.center.y + sizeY));
	glEnd();
}

/**
 * Draw a ball at it's current location.
 * @param	Ball	ball	the ball to draw
 */
void drawBall(Ball ball) {
		int i, sides = 32;
		float angle;

	glColor3f(ball.color.r, ball.color.g, ball.color.b);

	glBegin(GL_POLYGON);
		for(i = 0; i < sides; ++i){
			angle =  i * 2*PI / sides;
			glVertex2f(
				(cos(angle) * ball.radius) + ball.origin.x,
				(sin(angle) * ball.radius) + ball.origin.y
			);
		}
	glEnd();
}

/**
 * Draw a normalized brick. (x = 0, y = 0) (top left corner)
 * @param	Brick	br					the current brick to draw
 */
void drawBrick(Brick br) {
	int index = defineBrickColor(br);
		glEnable(GL_TEXTURE_2D);
		glColor3f(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[index]);

		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0, 0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f((BRICK_WIDTH - 1), 0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f((BRICK_WIDTH - 1), (BRICK_HEIGHT - 1));
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(0, (BRICK_HEIGHT - 1));
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texturesBuffer[index]);
		glDisable(GL_TEXTURE_2D);
}

/**
 * Draw a 2 dimensional grid of bricks, centered, based on their type and index.
 * @param	GridBrick	grid				the current gridBrick to draw
 * @param	int				gridWidth		the gridWidth from the config file
 * @param	int				gridHeight	the gridHeight from the config file
 */
void drawGrid(GridBrick const grid,int gridWidth, int gridHeight) {
	int i, j;
	int originX = SCREEN_WIDTH_CENTER - ((gridWidth / 2) * BRICK_WIDTH);
	if (gridWidth % 2 == 1) {
		originX -= (BRICK_WIDTH / 2);
	}
	int originY = (SCREEN_HEIGHT_CENTER - ((gridHeight) / 2) * BRICK_HEIGHT);
	if (gridHeight % 2 == 1) {
		originY -= (BRICK_HEIGHT / 2);
	}

	for (i = 0; i < gridHeight; ++i) {
		for (j = 0; j < gridWidth; ++j) {
			if (grid[i][j].status != DESTROYED) {
				glPushMatrix();
					glTranslatef((originX + (j * (BRICK_WIDTH - 1))), (originY + (i * (BRICK_HEIGHT - 1))), 0);
					drawBrick(grid[i][j]);
				glPopMatrix();
			}
		}
	}
}

/**
 * Draw the background texture. (x = 0, y = 0) (top left corner)
 * @param	int	index	the correct texture index needed to draw the brick
 */
void drawBackground(int index) {
		glEnable(GL_TEXTURE_2D);
		glColor3f(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[index]);

		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(SCREEN_WIDTH , 0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0, SCREEN_HEIGHT);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texturesBuffer[index]);
		glDisable(GL_TEXTURE_2D);
}

/*/////////////////////////////////////////
 //					HUD DISPLAY FUNCTIONS				//
/////////////////////////////////////////*/
/**
 * Draw a single HUD based on it's corners
 * @param	int			index				index Texture
 * @param	Point2D	topLeft			top left corner
 * @param	Point2D	topRight		top right corner
 * @param	Point2D	bottomRight	bottom right corner
 * @param	Point2D	bottomLeft	bottom left corner
 */
void drawRectangle(int index, Point2D topLeft, Point2D topRight, Point2D bottomRight, Point2D bottomLeft) {
	glEnable(GL_TEXTURE_2D);
	glColor3f(255, 255, 255);
	glBindTexture(GL_TEXTURE_2D, texturesBuffer[index]);

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(topLeft.x, topLeft.y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(topRight.x, topRight.y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(bottomRight.x, bottomRight.y);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(bottomLeft.x, bottomLeft.y);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texturesBuffer[index]);
	glDisable(GL_TEXTURE_2D);
}


/**
 * Draw all HUD based on the number of players. Automatically up to date
 * because It's based on the Players structures
 * @param	Player const*	pl				The current player
 * @param	int						nbPlayers	total number of players in game
 */
void drawHUD(Player const *pl, int nbPlayers) {
	char score[9] = "0";
	sprintf(score, "%d", pl->score);
	Point2D topLeft, topRight, bottomRight, bottomLeft;

	if (pl->id == 1) {
		initPoint2D(&topLeft, 0, 0);
		initPoint2D(&topRight, SCREEN_WIDTH, 0);
		initPoint2D(&bottomRight, SCREEN_WIDTH, HUD_HEIGHT);
		initPoint2D(&bottomLeft, 0, HUD_HEIGHT);

		drawRectangle(5, topLeft, topRight, bottomRight, bottomLeft);

		glPushMatrix();
			glTranslatef((LIFE_SIZE_WIDTH * 2), ((HUD_HEIGHT / 2) - (LIFE_SIZE_HEIGHT / 2)), 0);
			drawLifes(pl->life);
		glPopMatrix();

		glColor3f(5, 11, 11);
		glPushMatrix();
			renderBitmapString(SCREEN_WIDTH_CENTER, (HUD_HEIGHT / 2)+5, pl->name);
			renderBitmapString((SCREEN_WIDTH - 120), (HUD_HEIGHT / 2)+7, score);
		glPopMatrix();

	} else if (pl->id == 2) {
		initPoint2D(&topLeft, 0, SCREEN_HEIGHT - HUD_HEIGHT);
		initPoint2D(&topRight, SCREEN_WIDTH, SCREEN_HEIGHT - HUD_HEIGHT);
		initPoint2D(&bottomRight, SCREEN_WIDTH, SCREEN_HEIGHT);
		initPoint2D(&bottomLeft, 0, SCREEN_HEIGHT);

		drawRectangle(5, topLeft, topRight, bottomRight, bottomLeft);

		glBindTexture(GL_TEXTURE_2D, texturesBuffer[5]);
		glDisable(GL_TEXTURE_2D);

		glPushMatrix();
			glTranslatef((LIFE_SIZE_WIDTH * 2), (SCREEN_HEIGHT - (HUD_HEIGHT / 2) - (LIFE_SIZE_HEIGHT / 2)), 0);
			drawLifes(pl->life);
		glPopMatrix();

		glColor3f(5, 11, 11);
		glPushMatrix();
			renderBitmapString(SCREEN_WIDTH_CENTER, (SCREEN_HEIGHT - (HUD_HEIGHT / 2))+5, pl->name);
			renderBitmapString((SCREEN_WIDTH - 120), (SCREEN_HEIGHT - (HUD_HEIGHT / 2))+7, score);
		glPopMatrix();
	}
	if (nbPlayers > 2) {
		if (pl->id == 3) {
			initPoint2D(&topLeft, 0, 0);
			initPoint2D(&topRight, HUD_HEIGHT, 0);
			initPoint2D(&bottomRight, HUD_HEIGHT, SCREEN_HEIGHT);
			initPoint2D(&bottomLeft, 0, SCREEN_HEIGHT);

			drawRectangle(15, topLeft, topRight, bottomRight, bottomLeft);

			glPushMatrix();
				glTranslatef(((HUD_HEIGHT / 2) + (LIFE_SIZE_HEIGHT / 2)), -20, 0);
				glRotatef(90, 0, 0, 1);
				drawLifes(pl->life);
			glPopMatrix();

			glColor3f(5, 11, 11);
			glPushMatrix();
				renderBitmapVerticalString(((HUD_HEIGHT / 2)-5), (SCREEN_HEIGHT_CENTER + 30), pl->name);
				renderBitmapVerticalString(((HUD_HEIGHT / 2)-5), (SCREEN_HEIGHT - 90), score);
			glPopMatrix();

		} else if (pl->id == 4) {
			initPoint2D(&topLeft, SCREEN_WIDTH - HUD_HEIGHT, 0);
			initPoint2D(&topRight, SCREEN_WIDTH, 0);
			initPoint2D(&bottomRight, SCREEN_WIDTH, SCREEN_HEIGHT);
			initPoint2D(&bottomLeft, SCREEN_WIDTH - HUD_HEIGHT, SCREEN_HEIGHT);

			drawRectangle(15, topLeft, topRight, bottomRight, bottomLeft);

			glPushMatrix();
				glTranslatef((SCREEN_WIDTH - (HUD_HEIGHT / 2 ) + (LIFE_SIZE_HEIGHT / 2)), -20, 0);
				glRotatef(90, 0, 0, 1);
				drawLifes(pl->life);
			glPopMatrix();

			glColor3f(5, 11, 11);
			glPushMatrix();
				renderBitmapVerticalString((SCREEN_WIDTH - (HUD_HEIGHT / 2)-5), (SCREEN_HEIGHT_CENTER + 30), pl->name);
				renderBitmapVerticalString((SCREEN_WIDTH - (HUD_HEIGHT / 2)-5), (SCREEN_HEIGHT - 90), score);
			glPopMatrix();
		}
	}
}

/**
 * Draw a single life
 */
void drawLife() {
	Point2D topLeft, topRight, bottomRight, bottomLeft;

	initPoint2D(&topLeft, 0, 0);
	initPoint2D(&topRight, LIFE_SIZE_WIDTH, 0);
	initPoint2D(&bottomRight, LIFE_SIZE_WIDTH, LIFE_SIZE_HEIGHT);
	initPoint2D(&bottomLeft, 0, LIFE_SIZE_HEIGHT);

	drawRectangle(6, topLeft, topRight, bottomRight, bottomLeft);
}

/**
 * Draw all lifes of one particular player.
 * based on it's current number of lifes
 * @param	int	nbHearts	number of remaining lifes
 */
void drawLifes(int nbHearts) {
	int i;
	for (i = nbHearts; i > 0; --i) {
		glPushMatrix();
		glTranslatef(47 +(i * (LIFE_SIZE_WIDTH +4)), 0, 0);
			drawLife();
		glPopMatrix();
	}
}

/**
 * Render an HORIZONTAL Bitmap string using GLUT library
 * @param	float				x				top left corner (x value)
 * @param	float				y				top left corner (y value)
 * @param	char const*	string	the string to render horizontally
 */
void renderBitmapString(float x, float y, char const *string) {
	char const *c;
	for (c = string; *c != '\0'; c++) {}
	int strLenght = c - string;
	glRasterPos2f(x - ((9 * strLenght) / 2), y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
	}
}

/**
 * Render a VERTICAL Bitmap string using GLUT library
 * @param	float				x				top left corner (x value)
 * @param	float				y				top left corner (y value)
 * @param	char const*	string	the string to render vertically
 */
void renderBitmapVerticalString(float x, float y, char const *string) {
	char const *c;
	for (c = string; *c != '\0'; c++) {}
	for (c = string; *c != '\0'; c++) {
		glRasterPos2f(x, y + ((15 *  (c - string) )));
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

	}
}

/**
 * Print the victory screen. Winner on top the all players in their ID order.
 * @param	Player const*	players		the players array containing all in game players
 * @param	int						nbPlayers	total number of players in game
 * @param	bool					gladOS		if GaldOS is active then print
 */
void printVictoryScreen(Player const *players, int nbPlayers, bool gladOS) {
	char scorePl[100];
	int score = players[0].score + (players[0].life * 10), tmpScore = 0;
	int index = 0;
	int i;

	glColor3f(255, 255, 255);
	glPushMatrix();
	if (nbPlayers < 3) {
		if (players[0].life == 0) {
			renderBitmapString((SCREEN_WIDTH_CENTER - (((NameLenght(players[1].name)/2) * 9))), 230, players[1].name);
		} else {
			renderBitmapString((SCREEN_WIDTH_CENTER - (((NameLenght(players[0].name)/2) * 9))), 230, players[0].name);
		}
		for (i = 0; i < 2; ++i) {
			sprintf(scorePl, "%s.........................%d", players[i].name, players[i].score);
			renderBitmapString((SCREEN_WIDTH_CENTER - (((NameLenght(players[i].name)/2) * 9))), (400 + ( 30 * (i + 1))), scorePl);
		}
	}

	if (nbPlayers > 2 ) {
		for (i = 1; i < nbPlayers; ++i) {
			tmpScore = players[i].score + (players[i].life * 10);
			if (tmpScore > score && players[i].life > 0) {
				score = tmpScore;
				index = i;
			}
		}
		renderBitmapString((SCREEN_WIDTH_CENTER - (((NameLenght(players[index].name)/2) * 9))), 230, players[index].name);
		for (i = 0; i < 4; ++i) {
			sprintf(scorePl, "%s.........................%d", players[i].name, players[i].score);
			renderBitmapString((SCREEN_WIDTH_CENTER - (((NameLenght(players[i].name)/2) * 9))), (400 + ( 30 * (i + 1))), scorePl);
		}
	}
	glPopMatrix();
}

/**
 * Calcul the user name length and return this length
 * @param		char const*	name	the player name
 * @return	int								lenght of the player name
 */
int NameLenght(char const *name) {
	int lenght = 0;
	while (name[lenght] != '\0') {
		lenght++;
	}
	return lenght;
}

/*/////////////////////////////////////////
 //					TEXTURES FUNCTIONS					//
/////////////////////////////////////////*/

/**
 * Return the color format of the picture
 * @param		SDL_Surface	img the picture currently loaded
 * @return	GLenum					format of the current picture
 */
GLenum testFormat(SDL_Surface *img){
	GLenum format;
	switch(img->format->BytesPerPixel){
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			return EXIT_FAILURE;
	}
	return format;
}

/**
 * Load and give all theme pictures to the GPU
 * @param	char 	themePath	the name of the theme directory
 */
void loadTextures(char *themePath) {
	SDL_Surface *surface;
	int i;
	char *tmp = themePath;
	char imgPath[100];

	for(i = 1; i <= TEXTURE_NB; ++i){
		switch (i) {
			case 1 :
			case 2 :
			case 3 :
			case 4 :
				sprintf(imgPath, "%sbrick0%d.jpg",tmp, i);
				break;
			case 5 :
				sprintf(imgPath, "%sbackground.jpg", tmp);
				break;
			case 6 :
				sprintf(imgPath, "%sHUDh.jpg", tmp);
				break;
			case 7 :
				sprintf(imgPath, "%slife.jpg", tmp);
				break;
			case 8 :
			case 9 :
			case 10 :
			case 11 :
			case 12 :
			case 13 :
				sprintf(imgPath, "img/button0%d.jpg", (i - 7));
				break;
			case 14 :
				sprintf(imgPath, "img/menu.jpg");
				break;
			case 15 :
				sprintf(imgPath, "img/victory.jpg");
				break;
			case 16 :
				sprintf(imgPath, "%sHUDv.jpg", tmp);
				break;
			case 17 :
				sprintf(imgPath, "img/pause.jpg");
				break;
			default :
				break;
		}

		/* If "imgPath" link is wrong (picture doesn't exist or themePath is wrong), segmentation error */
		surface = IMG_Load(imgPath);

		if(surface == NULL) {
			printf("Error : can't load the picture '%s' from the '%s' theme : memory issue.", imgPath, themePath);
		}

		/* Génération des identifiants des textures */
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[i - 1]);
			/* Association des textures avec les identifiants*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, texturesBuffer[i - 1]);
			GLenum format = testFormat(surface);
			glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	SDL_FreeSurface(surface);
}
