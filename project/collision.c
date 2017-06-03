/**
 * @file	collision.c
 *       	Collision functions library.Find collisions, apply/change basic objects speed.
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date	2017-04-27
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "headers.h"

/*/////////////////////////////////////////
 //		BASIC COLLISION FUNCTIONS 		//
/////////////////////////////////////////*/

/**
 * determines if there is a collision between a ball and the screen borders.
 * @param	Ball*	ball	the current ball pointer
 */
void collisionBallScreen(Ball *ball) {
	if ((ball->origin.x - ball->radius) <= 0 || (ball->origin.x + ball->radius) >= SCREEN_WIDTH) {
		ball->speed.x *= -1;
	}
	if ((ball->origin.y - ball->radius) <= HUD_HEIGHT) {
		ballOutOfBounds(ball, TOP);
	}
	if ((ball->origin.y + ball->radius) >= SCREEN_HEIGHT - HUD_HEIGHT) {
		ballOutOfBounds(ball, BOTTOM);
	}
}

/**
 * Determines if there is a collision between a ball and a line.
 * @param	Ball*	ball	the current ball pointer
 * @param	Point2D	A		point A of the line
 * @param	Point2D	B		point B of the line
 * @return					return true if there is a collision, false otherwise
 */
bool collisionBallLine(Ball const *ball, Point2D A, Point2D B) {
	Vector2D AB = defineVector(A, B);
	Vector2D AC = defineVector(A, ball->origin);

	float num = abs((AB.x * AC.y) - (AB.y * AC.x));
	float denum = norm(AB);
	float CI = num / denum;

	return CI < ball->radius;
}

/**
 * Determines if there is a collision between a ball and a segment (one brick segment for exemple).
 * It uses the dot product to calculate if there is a collision.
 * Otherwithe, it uses the ball radius to calculate the distance between the ball and either A or B points.
 * @param	Ball*	ball	the current ball pointer
 * @param	Point2D	A		point A of the segment (start)
 * @param	Point2D	B		point B of the segment (end)
 * @return					return true if there is a collision, false otherwise
 */
enum collisionType collisionBallSegment(Ball const *ball, Point2D A, Point2D B) {
	if (!collisionBallLine(ball, A, B)) {
		return NO_COLLISION;
	}
	Vector2D AB, AC, BC;
	AB = defineVector(A, B);
	AC = defineVector(A, ball->origin);
	BC = defineVector(B, ball->origin);

	float dotScal1 = dotPRoduct(AB, AC);
	float dotScal2 = (-1) * dotPRoduct(AB, BC);

	if (dotScal1 >= 0 && dotScal2 >= 0) {
		return SEGMENT;
	}
	if (distance(ball->origin, A) < ball->radius) {
		return CORNER_A;
	}
	if (distance(ball->origin, B) < ball->radius) {
		return CORNER_B;
	}
	return NO_COLLISION;
}

/**
 * Determines if there is a collision between a ball and a brick.
 * @param	Ball*	ball	the current ball pointer
 * @param	Brick*	brick	the current brick pointer
 * @return	enum			return the collided side label
 */
enum direction collisionBallBrick(Ball const *ball, Brick const *brick) {
	enum collisionType collision = NO_COLLISION;
	if (ball->speed.y < 0) {
		if ((collision = collisionBallSegment(ball, brick->bottomRight, brick->bottomLeft))) {
			if (collision == SEGMENT) {
				return BOTTOM;
			} else if (collision == CORNER_A) {
				return BOTTOM_RIGHT;
			} else if (collision == CORNER_B) {
				return BOTTOM_LEFT;
			}
		}
	}
	if (ball->speed.y > 0) {
		if ((collision = collisionBallSegment(ball, brick->topRight, brick->topLeft))) {
			if (collision == SEGMENT) {
				return TOP;
			} else if (collision == CORNER_A) {
				return TOP_LEFT;
			} else if (collision == CORNER_B) {
				return TOP_RIGHT;
			}
		}
	}
	if (ball->speed.x < 0) {
		if((collision = collisionBallSegment(ball, brick->bottomRight, brick->topRight))) {
			if (collision == SEGMENT) {
				return RIGHT;
			} else if (collision == CORNER_A) {
				return BOTTOM_RIGHT;
			} else if (collision == CORNER_B) {
				return BOTTOM_LEFT;
			}
		}
	}
	if (ball->speed.x > 0) {
		if((collision = collisionBallSegment(ball, brick->topLeft, brick->bottomLeft))) {
			if (collision == SEGMENT) {
				return LEFT;
			} else if (collision == CORNER_A) {
				return TOP_LEFT;
			} else if (collision == CORNER_B) {
				return TOP_RIGHT;
			}
		}
	}
	return NONE;
}

/**
 * Determines if there is a collision between a ball and one brick of the grid.
 * Change the ball speed depending on wich side it collided the brick.
 * @param	GridBrick	grid		the 2 dimensional brick grid
 * @param	Ball*		ball		the current ball pointer
 * @param	int			gridWidth	the config file gridWidth
 * @param	int			gridHeight	the config file gridHeight
 * @return							return true if there is a collision, false otherwise
 */
bool collisionBallGrid(GridBrick grid, Ball *ball, int gridWidth, int gridHeight) {
	int i, j;
	enum direction collision;

	for (i = 0; i < gridHeight; ++i) {
		for (j = 0; j < gridWidth; ++j) {
			if (grid[i][j].status != DESTROYED) {
				collision = collisionBallBrick(ball, &grid[i][j]);
				if (collision != NONE) {
					hitBrick(&grid[i][j], ball);
					if (collision == TOP || collision == BOTTOM) {
						ball->speed.y *= -1;
					}
					if (collision == LEFT || collision == RIGHT) {
						ball->speed.x *= -1;
					}
					if (collision == TOP_LEFT || collision == TOP_RIGHT ||
						collision == BOTTOM_LEFT || collision == BOTTOM_RIGHT) {
						ball->speed.y *= -1;
						ball->speed.x *= -1;
					}
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * Determines if there is a collision between a bar and a ball.
 * Change the ball speed according depending o wich side it collided with the bar.
 * @param	Bar*	bar		the current bar pointer
 * @param	Ball*	ball	the current ball pointer
 */
void collisionBarBall(Bar const *bar, Ball *ball) {
	Point2D topLeft, topRight, bottomLeft, bottomRight;
	enum collisionType collision;

	topLeft.x = bar->center.x - bar->width / 2;
	topLeft.y = bar->center.y - BAR_HEIGHT / 2;

	topRight.x = bar->center.x + bar->width / 2;
	topRight.y = bar->center.y - BAR_HEIGHT / 2;

	bottomLeft.x = bar->center.x - bar->width / 2;
	bottomLeft.y = bar->center.y + BAR_HEIGHT / 2;

	bottomRight.x = bar->center.x + bar->width / 2;
	bottomRight.y = bar->center.y + BAR_HEIGHT / 2;

	if (ball->speed.y < 0) {
		if ((collision = collisionBallSegment(ball, bottomRight, bottomLeft))) {
			if (collision == SEGMENT) {
				ball->speed.y *= -1;
			} else if (collision == CORNER_A || collision == CORNER_B) {
				ball->speed.x *= -1;
				ball->speed.y *= -1;
			}
			ball->lastPlayerId = bar->playerId;
		}
	}
	if (ball->speed.y > 0) {
		if ((collision = collisionBallSegment(ball, topRight, topLeft))) {
			if (collision == SEGMENT) {
				ball->speed.y *= -1;
			} else if (collision == CORNER_A || collision == CORNER_B) {
				ball->speed.x *= -1;
				ball->speed.y *= -1;
			}
			ball->lastPlayerId = bar->playerId;
		}
	}
	if (ball->speed.x < 0) {
		if((collision = collisionBallSegment(ball, bottomRight, topRight))) {
			if (collision == SEGMENT) {
				ball->speed.x *= -1;
			} else if (collision == CORNER_A || collision == CORNER_B) {
				ball->speed.x *= -1;
				ball->speed.y *= -1;
			}
			ball->lastPlayerId = bar->playerId;
		}
	}
	if (ball->speed.x > 0) {
		if((collision = collisionBallSegment(ball, topLeft, bottomLeft))) {
			if (collision == SEGMENT) {
				ball->speed.x *= -1;
			} else if (collision == CORNER_A || collision == CORNER_B) {
				ball->speed.x *= -1;
				ball->speed.y *= -1;
			}
			ball->lastPlayerId = bar->playerId;
		}
	}
}

/*/////////////////////////////////////////
 //		BASIC MOUVEMENTS FUNCTIONS 		//
/////////////////////////////////////////*/

/**
 * Set the ball origin point to the new state, depending on it's speed.
 * @param	Ball*	ball	the current ball pointer
 */
void moveBall(Ball *ball) {
	ball->origin.x += ball->speed.x;
	ball->origin.y += ball->speed.y;
}
