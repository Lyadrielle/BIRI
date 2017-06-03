/**
 * @file	gameplay.c
 *       	gameplay functions library. Centralise all the gameplay functions,
 * 			definitions/initialisations depending on configuration files or game rules.
 * 			mouvement, bonus, life, score...
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date	2017-05-07
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

#include "headers.h"

/*/////////////////////////////////////////
 //			GLOBAL VARIABLES DEF		//
/////////////////////////////////////////*/

Ball *balls;
Player *players;

/*/////////////////////////////////////////
 //	GAMEPLAY INITIALISATON FUNCTIONS	//
/////////////////////////////////////////*/

void initPlayer(Player *pl, int id, char *name, Point2D barCenter, Color3f barColor) {
	pl->id = id;
	pl->name = name;
	initBar(&(pl->bar), barCenter, barColor, id);
	if (strcmp(name, "cat") == 0) {
		pl->life = 9;
	} else {
		pl->life = START_LIFE;
	}
	pl->immune = false;
	pl->score = 0;
}

void initGame(int nbPlayers) {
	players = malloc(nbPlayers * sizeof(Player));
	if (players == NULL) {
		exit(MALLOC_ERROR);
	}

	balls = malloc(nbPlayers * sizeof(Ball));
	if (balls == NULL) {
		exit(MALLOC_ERROR);
	}

	int i;
	Point2D barCenter, ballCenter;
	Vector2D ballSpeed;
	Color3f barColor, ballColor;

	for (i = 1; i <= nbPlayers; ++i) {
		switch (i) {
		case 1 :
			initPoint2D(&barCenter, (screenWidthCenter - (BASIC / 2)), (HUD_HEIGHT + BAR_HEIGHT));
			initColor3f(&barColor, 255, 0, 0);
			initPoint2D(&ballCenter, (screenWidthCenter - (BASIC / 2)), (HUD_HEIGHT + (3 * BAR_HEIGHT)));
			initVector2D(&ballSpeed, NORMAL, NORMAL);
			initColor3f(&ballColor, 255, 0, 0);
			break;
		case 2 :
			initPoint2D(&barCenter, (screenWidthCenter - (BASIC / 2)), (GAME_HEIGHT - HUD_HEIGHT - BAR_HEIGHT));
			initColor3f(&barColor, 0, 0, 255);
			initPoint2D(&ballCenter, (screenWidthCenter - (BASIC / 2)), (GAME_HEIGHT - HUD_HEIGHT - (3 * BAR_HEIGHT)));
			initVector2D(&ballSpeed, NORMAL, (NORMAL * -1));
			initColor3f(&ballColor, 0, 0, 255);
			break;
		default :
			break;
		}
		initPlayer(&players[i-1], i, "TRAPATAPUSHPUTASSISTATUR", barCenter, barColor);
		initBall(&balls[i-1], i, BALL_RADIUS, ballSpeed, ballCenter, ballColor, i);
	}
}

/*/////////////////////////////////////////
 //		GAMEPLAY ACTION FUNCTIONS		//
/////////////////////////////////////////*/

/**
 * Apply changes of x position to a bar, depending on the player actions and the screen borders.
 * @param	Bar*	bar	the current bar pointer
 * @param	enum	dir	the screen border
 */
void moveBar (Bar *bar, enum direction dir) {
  if (dir == LEFT) {
    if ((bar->center.x - (bar->width / 2)) >= 0) {
      bar->center.x -= BAR_SPEED;
    }
  }
  if (dir == RIGHT) {
    if ((bar->center.x + (bar->width / 2)) <= screenWidth) {
      bar->center.x += BAR_SPEED;
    }
  }
}

/**
 * Start all imediate actions related to a brick being hit by a ball.
 * Set the current statud from PRISTINE or DAMAGED to DAMAGED or DESTROYED.
 * @param	Brick*	brick	the current brick pointer
 */
void hitBrick (Brick *brick, Ball *ball) {
	int barWidth = players[ball->lastPlayerId - 1].bar.width;
	if (brick->type != INDESTRUCTIBLE) {
		brick->status = DESTROYED;
		players[ball->lastPlayerId - 1].score += 10;
	}
	if (brick->type == WIDER_BAR) {
		if (barWidth == SMALL) {
			players[ball->lastPlayerId - 1].bar.width = BASIC;
		} else {
			players[ball->lastPlayerId - 1].bar.width = LARGE;
		}
	}
	if (brick->type == SMALLER_BAR) {
		if (barWidth == LARGE) {
			players[ball->lastPlayerId - 1].bar.width = BASIC;
		} else {
			players[ball->lastPlayerId - 1].bar.width = SMALL;
		}
	}
	if (brick->type == ADD_LIFE) {
		++(players[ball->lastPlayerId - 1].life);
	}

	if (brick->type == SLOWER_BALL) {
		ball->speed.x = ball->speed.x == NORMAL ? SLOW : NORMAL;
		ball->speed.y = ball->speed.y == NORMAL ? SLOW : NORMAL;
	}
	if (brick->type == FASTER_BALL) {
		ball->speed.x = ball->speed.x == NORMAL ? FAST : NORMAL;
		ball->speed.y = ball->speed.y == NORMAL ? FAST : NORMAL;
	}
	if (brick->type != ORDINARY) {
		ball->bonusTimer = BALL_BONUS_TIME;
	}
}

/**
 * Start all imediate actions related to a ball falling out of the playground.
 * (TO DO) lower the targeted player's life by 1.
 * Relaunch the ball at the center of the screen, on the side it fell.
 * @param	Ball*	ball	the current ball pointer
 * @param	enum	dir		the side the ball fell
 */
void ballOutOfBounds(Ball *ball, enum direction dir) {
	ball->origin.x = screenWidthCenter;
	ball->respawnTimer = BALL_RESPAWN_TIME;
	ball->speed.y *= -1;
	if (dir == TOP) {
		--(players[0].life);
		ball->lastPlayerId = 0;
		ball->origin.y = HUD_HEIGHT + (3 * BAR_HEIGHT);
	}
	if (dir == BOTTOM) {
		--(players[1].life);
		ball->lastPlayerId = 1;
		ball->origin.y = GAME_HEIGHT - HUD_HEIGHT - (3 * BAR_HEIGHT);
	}
}

/*/////////////////////////////////////////
 //		GAMEPLAY BRICK FUNCTIONS		//
/////////////////////////////////////////*/

/**
 * Define a brick color (TO DO TEXTURES) based on it's type.
 * @param	Brick	br	the current brick to look at
 */
void defineBrickColor(Brick br) {
	switch (br.type) {
		case 0 :
			glColor3f(255, 255, 255);
			break;
		case 1 :
			glColor3f(0, 0, 255);
			break;
		case 2 :
		case 4 :
		case 6 :
			glColor3f(0, 255, 0);
			break;
		case 3 :
		case 5 :
			glColor3f(255, 0, 0);
			break;
		default :
			glColor3f(255, 255, 255);
			break;
	}
}
