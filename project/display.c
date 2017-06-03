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
#include <SDL/SDL_image.h>

#include "headers.h"

/*/////////////////////////////////////////
 //		BASIC DRAWING FUNCTIONS			//
/////////////////////////////////////////*/

/**
 * Draw a centerd bar based on the player's id.
 * @param	Bar	bar	the current bar to draw
 */
void drawBar(Bar bar, GLuint *textures) {
	if(bar.playerId < TEXTURE_NB){
		glEnable(GL_TEXTURE_2D);
		glColor3f(255,255,255);
		glBindTexture(GL_TEXTURE_2D, textures[bar.playerId]);

		glBegin(GL_POLYGON);
		glTexCoord2f(0.0f,0.0f);
		glVertex2f((bar.center.x - bar.width / 2), (bar.center.y - BAR_HEIGHT / 2));
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f((bar.center.x + bar.width / 2), (bar.center.y - BAR_HEIGHT / 2));
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f((bar.center.x + bar.width / 2), (bar.center.y + BAR_HEIGHT / 2));
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f((bar.center.x - bar.width / 2), (bar.center.y + BAR_HEIGHT / 2));
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[bar.playerId]);
		glDisable(GL_TEXTURE_2D);

	}
	else {
		glColor3f(bar.color.r, bar.color.g, bar.color.b);
		glBegin(GL_POLYGON);
			glVertex2f((bar.center.x - bar.width / 2), (bar.center.y - BAR_HEIGHT / 2));
			glVertex2f((bar.center.x + bar.width / 2), (bar.center.y - BAR_HEIGHT / 2));
			glVertex2f((bar.center.x + bar.width / 2), (bar.center.y + BAR_HEIGHT / 2));
			glVertex2f((bar.center.x - bar.width / 2), (bar.center.y + BAR_HEIGHT / 2));
		glEnd();
	}
	
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
 * @param	GLuint 		textures 	array containing textures places	
 */
void drawBrick(Brick br, int brickWidth, GLuint textures[]) {
	if(br.type < TEXTURE_NB){
		glEnable(GL_TEXTURE_2D);
		glColor3f(255,255,255);
		glBindTexture(GL_TEXTURE_2D, textures[br.type]);

		glBegin(GL_POLYGON);
		glTexCoord2f(0.0f,0.0f);
		glVertex2f(0, 0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f((brickWidth - 1), 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f((brickWidth - 1), (BRICK_HEIGHT - 1));
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(0, (BRICK_HEIGHT - 1));
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[br.type]);
		glDisable(GL_TEXTURE_2D);

	}
	else {
		defineBrickColor(br);
		glBegin(GL_POLYGON);
		glVertex2f(0, 0);
		glVertex2f((brickWidth - 1), 0);
		glVertex2f((brickWidth - 1), (BRICK_HEIGHT - 1));
		glVertex2f(0, (BRICK_HEIGHT - 1));
		glEnd();
	}
}

/**
 * Draw a 2 dimensional grid of bricks, centered, based on their type and index.
 * @param	GridBrick	grid		the current gridBrick to draw
 * @param	int			gridWidth	the gridWidth from the config file
 * @param	int			gridHeight	the gridHeight from the config file
 * @param	int			brickWidth	the brickWidth based on the
 *              					gridWidth and screenWidth
 */
void drawGrid(GridBrick const grid,int gridWidth, int gridHeight, int brickWidth, GLuint textures[]) {
	int i, j;
	int origin = (SCREEN_HEIGHT / 2) - ((gridHeight * BRICK_HEIGHT) / 2);
	Point2D topLeft;

	for (i = 0; i < gridHeight; ++i) {
		for (j = 0; j < gridWidth; ++j) {
			if (grid[i][j].status != DESTROYED) {
				glPushMatrix();
					glTranslatef(((brickWidth * j)), (origin + (i * BRICK_HEIGHT)), 0);
					drawBrick(grid[i][j], brickWidth, textures);
				glPopMatrix();
			}
			topLeft.x = brickWidth * j;
			topLeft.y = origin + (i * BRICK_HEIGHT);
			updateBrickCoordinates(&grid[i][j], brickWidth, topLeft);
		}
	}
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
 * @param	GLuint 		textures 	array containing textures places
 * @param	char 		themeName	the name of the theme directory
 */
void loadTextures(GLuint *textures, char *themeName) {
	int nbBrickType = 5;
	SDL_Surface *surface;
	char *imgName = malloc(TEXTURE_NAME_SIZE * sizeof(char));
	char *imgNameBase = malloc(TEXTURE_NAME_SIZE * sizeof(char));
	char tempName[3];
	int i;

	/* Selection of the good theme*/
	if(strcmp(themeName, "")){ /* If theme name given */
		strcpy(imgNameBase, "img/");		
		strcat(imgNameBase, themeName);
		strcat(imgNameBase, "/brique_");
	}
	else {		/* else Default theme */
		strcpy(imgNameBase, "img/Default/brique_");		
		strcpy(themeName, "Default");
	}

	for(i = 0; i < TEXTURE_NB; ++i){
		strcpy(imgName, imgNameBase);
		sprintf(tempName, "%d", i);
		strcat(imgName, tempName);
		strcat(imgName, ".jpg");

		/* If "imgName" link is wrong (picture doesn't exist or themeName is wrong), segmentation error */
		surface = IMG_Load(imgName);

		if(surface == NULL) printf("Error : can't load the picture '%s' from the '%s' theme : memory issue.", imgName, themeName);

		/* Génération des identifiants des textures */
		glBindTexture(GL_TEXTURE_2D, textures[i]);

		/* Association des textures avec les identifiants*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		GLenum format = testFormat(surface);
		glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	SDL_FreeSurface(surface);

	if (imgName != NULL) {
		free(imgName);
	}
}
