/**
 * @file	display.c
 *       	display functions library. Centralise all basic display functions,
 * 			basic shapes non dependent of the gameplay.
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <SDL/SDL_image.h>

#include "headers.h"

int totalPlayers;

/*/////////////////////////////////////////
 //		BASIC DRAWING FUNCTIONS			//
/////////////////////////////////////////*/

/**
 * Draw a centerd bar based on the player's id.
 * @param	Bar	bar	the current bar to draw
 */
void drawBar(Bar bar) {
	glColor3f(bar.color.r, bar.color.g, bar.color.b);
	glBegin(GL_POLYGON);
		glVertex2f((bar.center.x - bar.width / 2), (bar.center.y - BAR_HEIGHT / 2));
		glVertex2f((bar.center.x + bar.width / 2), (bar.center.y - BAR_HEIGHT / 2));
		glVertex2f((bar.center.x + bar.width / 2), (bar.center.y + BAR_HEIGHT / 2));
		glVertex2f((bar.center.x - bar.width / 2), (bar.center.y + BAR_HEIGHT / 2));
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
 * @param	Brick	br			the current brick to draw
 * @param	int		brickWidth	the brick width based on the
 *             					config file gridWidth and screenWidth
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
 * @param	GridBrick	grid		the current gridBrick to draw
 * @param	int			gridWidth	the gridWidth from the config file
 * @param	int			gridHeight	the gridHeight from the config file
 * @param	int			brickWidth	the brickWidth based on the
 *              					gridWidth and screenWidth
 */
void drawGrid(GridBrick const grid,int gridWidth, int gridHeight) {
	int i, j;
	int originX = (SCREEN_WIDTH / 2) - ((gridWidth / 2) * BRICK_WIDTH);
	int originY = ((SCREEN_HEIGHT / 2) - ((gridHeight) / 2) * BRICK_HEIGHT);

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
 *
 */
void drawBackground() {
		glEnable(GL_TEXTURE_2D);
		glColor3f(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[4]);

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

		glBindTexture(GL_TEXTURE_2D, texturesBuffer[4]);
		glDisable(GL_TEXTURE_2D);
}

void drawHUD(Player const *pl) {
	char score[9] = "0";
	sprintf(score, "%d", pl->score);

	if (pl->id == 1) {
		glEnable(GL_TEXTURE_2D);
		glColor3f(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[5]);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(SCREEN_WIDTH, 0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(SCREEN_WIDTH, HUD_HEIGHT);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0, HUD_HEIGHT);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[5]);
		glDisable(GL_TEXTURE_2D);


		glPushMatrix();
			glTranslatef((LIFE_SIZE_WIDTH * 2), ((HUD_HEIGHT / 2) - (LIFE_SIZE_HEIGHT / 2)), 0);
			drawLifes(pl->life);
		glPopMatrix();

		glColor3f(5, 11, 11);
		glPushMatrix();
			renderBitmapString((SCREEN_WIDTH / 2), (HUD_HEIGHT / 2)+5, pl->name);
		glPopMatrix();
		glPushMatrix();
			renderBitmapString((SCREEN_WIDTH - 120), (HUD_HEIGHT / 2)+7, score);
		glPopMatrix();

	} else if (pl->id == 2) {
		glEnable(GL_TEXTURE_2D);
		glColor3f(255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[5]);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0, SCREEN_HEIGHT - HUD_HEIGHT);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT - HUD_HEIGHT);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0, SCREEN_HEIGHT);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[5]);
		glDisable(GL_TEXTURE_2D);

		glPushMatrix();
			glTranslatef((LIFE_SIZE_WIDTH * 2), (SCREEN_HEIGHT - (HUD_HEIGHT / 2) - (LIFE_SIZE_HEIGHT / 2)), 0);
			drawLifes(pl->life);
		glPopMatrix();

		glColor3f(5, 11, 11);
		glPushMatrix();
			renderBitmapString((SCREEN_WIDTH / 2), (SCREEN_HEIGHT - (HUD_HEIGHT / 2))+5, pl->name);
		glPopMatrix();
		glPushMatrix();
			renderBitmapString((SCREEN_WIDTH - 120), (SCREEN_HEIGHT - (HUD_HEIGHT / 2))+7, score);
		glPopMatrix();
	}
}

void drawLife() {
	glEnable(GL_TEXTURE_2D);
	glColor3f(255, 255, 255);
	glBindTexture(GL_TEXTURE_2D, texturesBuffer[6]);

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(LIFE_SIZE_WIDTH, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(LIFE_SIZE_WIDTH, LIFE_SIZE_HEIGHT);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0, LIFE_SIZE_HEIGHT);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texturesBuffer[6]);
	glDisable(GL_TEXTURE_2D);
}


void drawLifes(int nbHearts) {
	int i;
	for (i = nbHearts; i > 0; --i) {
		glPushMatrix();
		glTranslatef(47 +(i * (LIFE_SIZE_WIDTH +4)), 0, 0);
			drawLife();
		glPopMatrix();
	}
}

void renderBitmapString(float x, float y, char const *string) {
	char const *c;
	for (c = string; *c != '\0'; c++) {}
	int strLenght = c - string;
	glRasterPos2f(x - ((9 * strLenght) / 2), y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
	}
}

void printVictoryScreen(Player const *players) {
	char scorePl[100];
	int i;

	glColor3f(255, 255, 255);
	glPushMatrix();
	switch (totalPlayers) {
		case 1 :
				if (players[0].life == 0) {
					renderBitmapString((SCREEN_WIDTH / 2), (GAME_HEIGHT / 2), "Ordinateur");
				} else {
					renderBitmapString((SCREEN_WIDTH / 2), (GAME_HEIGHT / 2), players[0].name);
				}
			break;
		case 2 :
			if (players[1].life == 0) {
				renderBitmapString((SCREEN_WIDTH / 2), (GAME_HEIGHT / 2), players[0].name);
			} else {
				renderBitmapString((SCREEN_WIDTH / 2), (GAME_HEIGHT / 2), players[1].name);
			}
			break;
		default :
			break;
	}
	for (i = 0; i < totalPlayers; ++i) {
		sprintf(scorePl, "%s : %d", players[i].name, players[i].score);
		renderBitmapString((SCREEN_WIDTH / 2), ((GAME_HEIGHT / 2) +( 30 * (i + 1))), scorePl);
	}
	glPopMatrix();
}

/*/////////////////////////////////////////
 //			TEXTURES FUNCTIONS			//
/////////////////////////////////////////*/

/**
 * Return the color format of the picture
 * @param	SDL_Surface  img the picture currently loaded
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
 * Load and give pictures of the theme to the GPU
 * @param	char 		themePath	the name of the theme directory
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
				sprintf(imgPath, "%sHUD.jpg", tmp);
				break;
			case 7 :
				sprintf(imgPath, "%slife.jpg", tmp);
				break;
			default :
				sprintf(imgPath, "%serror.jpg", tmp);
				break;
		}
		printf("PATH : %s et %d\n", imgPath, i-1);

		/* If "imgPath" link is wrong (picture doesn't exist or themePath is wrong), segmentation error */
		surface = IMG_Load(imgPath);

		if(surface == NULL) {
			printf("Error : can't load the picture '%s' from the '%s' theme : memory issue.", imgPath, themePath);
		}

		/* Génération des identifiants des textures */
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[i - 1]);
		/* Association des textures avec les identifiants*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, texturesBuffer[i - 1]);
		GLenum format = testFormat(surface);
		glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	SDL_FreeSurface(surface);
}
