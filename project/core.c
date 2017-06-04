/**
 * @file	core.c
 *       	Core functions library. Read config files, initiate basic structures,
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date	2017-04-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "headers.h"

/**
 * Read the config file containing the grid configuration.
 * @param	char*	filePath	the relative file path
 * @param	int*	gridWidth	the pointer of the gridWidth
 * @param	int*	gridHeight	the pointer of the gridHeight
 * @return						return the array containing all brick types in order
 */
int *readConfigFile(char *filePath, int *gridWidth, int *gridHeight) {
	FILE *file;
	char tmp[500];
	int *brickTypes;
	int i;

	if((file = fopen(filePath, "r")) == NULL) {
		printf("ERROR : Impossible to read the config file.\n");
		exit(1);
	}

	fscanf(file, " %d %d\n", gridWidth, gridHeight);
	*gridWidth = *gridWidth > 15 ? 15 : *gridWidth;
	*gridHeight = *gridHeight > 9 ? 9 : *gridHeight;
	*gridWidth = *gridWidth < 0 ? 0 : *gridWidth;
	*gridHeight = *gridHeight < 0 ? 0 : *gridHeight;


	fgets(tmp, sizeof(tmp), file);

	brickTypes = malloc(*gridWidth * *gridHeight * sizeof(int));
	if (brickTypes == NULL) {
		exit(MALLOC_ERROR);
	}

	char *token = strtok(tmp, " ");

	for (i = 0; i < (*gridWidth * *gridHeight); ++i) {
		brickTypes[i] = (int)(token[0] - '0');
		token = strtok(NULL, " ");
	}

	return brickTypes;
}

/*/////////////////////////////////////////
 //		INITIALISATION FUNCTIONS		//
/////////////////////////////////////////*/

/**
 * Initialise a bar for a player structure.
 * @param	Bar*	bar		the pointer of the bar to be initialised
 * @param	Point2D	center	the origin of the bar depending on the player
 * @param	Color3f	color	the bar color depending on the player too
 */
void initBar(Bar *bar, Point2D center, Color3f color, int playerId) {
	bar->playerId = playerId;

	bar->orientationHorizontal = (playerId == 1 || playerId == 2);

	bar->width = BASIC;

	bar->center.x = center.x;
	bar->center.y = center.y;

	bar->color.r = color.r;
	bar->color.g = color.g;
	bar->color.b = color.b;
}

/**
 * Initialise a ball.
 * @param	Ball*		bl		the pointer of the ball to be initialised
 * @param	int			id		the ball id (multiple balls allowed in the game)
 * @param	int			radius	the ball radius
 * @param	Vector2D	speed	the ball speed depending on the player (start position)
 * @param	Point2D		origin	the origin of the ball depending on the player too
 * @param	Color3f		color	the ball color depending on the player (for now)
 */
void initBall(Ball *bl, int id, int radius, Vector2D speed, Point2D origin, Color3f color, int lastPlayerId) {
	bl->id = id;
	bl->respawnTimer = BALL_RESPAWN_TIME;
	bl->bonusTimer = 0;

	bl->radius = radius;
	bl->origin.x = origin.x;
	bl->origin.y = origin.y;
	bl->speed.x = speed.x;
	bl->speed.y = speed.y;

	bl->color.r = color.r;
	bl->color.g = color.g;
	bl->color.b = color.b;

	bl->lastPlayerId = lastPlayerId;
}

/**
 * Initialise a brick with it's index in the grid of bricks.
 * @param	Brick*	b		the pointer of the brick to be initialied
 * @param	int		type	the type of the brick taken from the brickTypes array
 * @param	enum	status	the initial status of the brick PRISTINE normally
 * @param	int		indexX	the x index of  the brick into the brickGrid
 * @param	int		indexY	the y index of  the brick into the brickGrid
 */
void initBrick(Brick *b, int type, enum brickStatus status, int indexX, int indexY) {
	b->type = type;
	b->status = status;

	b->gridX = indexX;
	b->gridY = indexY;
}

/**
 * Update a brick structure to set the coordinates of each corners. (to avoid numerous operations)
 * @param	Brick*	br		the pointer of the brick to be updated
 * @param	int		width	the width of the brick
 * @param	Point2D	topLeft	the top left corner of the brick
 */
void updateBrickCoordinates(Brick *br, Point2D topLeft) {
	br->topLeft.x = topLeft.x;
	br->topLeft.y = topLeft.y;

	br->topRight.x = topLeft.x + BRICK_WIDTH;
	br->topRight.y = topLeft.y;

	br->bottomLeft.x = topLeft.x;
	br->bottomLeft.y = topLeft.y + BRICK_HEIGHT;

	br->bottomRight.x = topLeft.x + BRICK_WIDTH;
	br->bottomRight.y = topLeft.y + BRICK_HEIGHT;
}

void initBrickCoordinates(GridBrick grid, int gridWidth, int gridHeight) {
	Point2D topLeft;
	int originX = SCREEN_WIDTH_CENTER - ((gridWidth / 2) * BRICK_WIDTH);
	if (gridWidth % 2 == 1) {
		originX -= (BRICK_WIDTH / 2);
	}
	int originY = (SCREEN_HEIGHT_CENTER - ((gridHeight) / 2) * BRICK_HEIGHT);
	if (gridHeight % 2 == 1) {
		originY -= (BRICK_HEIGHT / 2);
	}

	int i, j;
	for (i = 0; i < gridHeight; ++i) {
		for (j = 0; j < gridWidth; ++j) {
			topLeft.x = originX + (j * BRICK_WIDTH);
			topLeft.y = originY + (i * BRICK_HEIGHT);
			updateBrickCoordinates(&grid[i][j], topLeft);
		}
	}
}

/**
 * Initialise a 2 dimensional grid of bricks with the config file params.
 * @param	int	gridWidth	the config file gridWidth
 * @param	int	gridHeight	the config file gridHeight
 * @param	int	blockType	the blockTypes array containing all bricks types
 * @return					return a 2 dimensional grid of bricks
 */
GridBrick initGrid(int gridWidth, int gridHeight, int *blockType) {
	GridBrick grid = malloc(gridHeight * sizeof(Brick*));
	if (grid == NULL) {
		exit(MALLOC_ERROR);
	}

	int i, j;
	for (i = 0; i <gridHeight; ++i) {
		grid[i] = malloc(gridWidth * sizeof(Brick));
		if (grid[i] == NULL) {
			exit(MALLOC_ERROR);
		}
		for (j = 0; j < gridWidth; ++j) {
			initBrick(&grid[i][j], blockType[i*gridWidth + j], PRISTINE, i, j);
		}
	}
	return grid;
}



/**
 * Read the config file containing the grid configuration.
 * @param	char*	filePath	the relative file path
 * @param	int*	gridWidth	the pointer of the gridWidth
 * @param	int*	gridHeight	the pointer of the gridHeight
 * @return						return the array containing all brick types in order
 */
int *readThemeFile(char *filePath, int *gridWidth, int *gridHeight) {
	FILE *file;
	char tmp[500];
	int *brickTypes;
	int i;

	if((file = fopen(filePath, "r")) == NULL) {
		printf("ERROR : Impossible to read the theme file.\n");
		exit(1);
	}

	fscanf(file, " %d %d\n", gridWidth, gridHeight);
	fgets(tmp, sizeof(tmp), file);

	brickTypes = malloc(*gridWidth * *gridHeight * sizeof(int));
	if (brickTypes == NULL) {
		exit(MALLOC_ERROR);
	}

	char *token = strtok(tmp, " ");

	for (i = 0; i < (*gridWidth * *gridHeight); ++i) {
		brickTypes[i] = (int)(token[0] - '0');
		token = strtok(NULL, " ");
	}

	return brickTypes;
}
