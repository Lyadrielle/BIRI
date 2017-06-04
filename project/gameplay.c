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
	if (nbPlayers == 1)	nbPlayers = 2;

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

	for (i = 1; i <= nbPlayers; ++i) {
		switch (i) {
		case 1 :
			initPoint2D(&barCenter, (SCREEN_WIDTH_CENTER - (BASIC / 2)), (HUD_HEIGHT + BAR_HEIGHT));
			initPoint2D(&ballCenter, (SCREEN_WIDTH_CENTER - (BASIC / 2)), (HUD_HEIGHT + (3 * BAR_HEIGHT)));
			initVector2D(&ballSpeed, NORMAL, NORMAL);
			break;
		case 2 :
			initPoint2D(&barCenter, (SCREEN_WIDTH_CENTER - (BASIC / 2)), (SCREEN_HEIGHT - HUD_HEIGHT - BAR_HEIGHT));
			initPoint2D(&ballCenter, (SCREEN_WIDTH_CENTER - (BASIC / 2)), (SCREEN_HEIGHT - HUD_HEIGHT - (3 * BAR_HEIGHT)));
			initVector2D(&ballSpeed, -NORMAL, -NORMAL);
			break;
		case 3 :
			initPoint2D(&barCenter, (HUD_HEIGHT + BAR_HEIGHT), (SCREEN_HEIGHT_CENTER - (BASIC / 2)));
			initPoint2D(&ballCenter, (HUD_HEIGHT + (3 * BAR_HEIGHT)), (SCREEN_HEIGHT_CENTER - (BASIC / 2)));
			initVector2D(&ballSpeed, NORMAL, -NORMAL);
			break;
		case 4 :
			initPoint2D(&barCenter, (SCREEN_WIDTH - HUD_HEIGHT - BAR_HEIGHT), (SCREEN_HEIGHT_CENTER - (BASIC / 2)));
			initPoint2D(&ballCenter, (SCREEN_WIDTH - HUD_HEIGHT - (3 * BAR_HEIGHT)), (SCREEN_HEIGHT_CENTER - (BASIC / 2)));
			initVector2D(&ballSpeed, -NORMAL, NORMAL);
			break;
		default :
			break;
		}
		initPlayer(&players[i-1], i,  playersNames[i - 1], barCenter, themeColor);
		initBall(&balls[i-1], i, BALL_RADIUS, ballSpeed, ballCenter, themeColor, i);
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
		if ((bar->center.x - (bar->width / 2)) >= (HUD_HEIGHT)) {
			bar->center.x -= BAR_SPEED;
		}
	}
	if (dir == RIGHT) {
		if ((bar->center.x + (bar->width / 2)) <= (SCREEN_WIDTH - HUD_HEIGHT)) {
			bar->center.x += BAR_SPEED;
		}
	}
	if (dir == TOP) {
		if ((bar->center.y - (bar->width / 2)) >= (HUD_HEIGHT)) {
			bar->center.y -= BAR_SPEED;
		}
	}
	if (dir == BOTTOM) {
		if ((bar->center.y + (bar->width / 2)) <= (SCREEN_HEIGHT - HUD_HEIGHT)) {
			bar->center.y += BAR_SPEED;
		}
	}
}

void handleGladOS(Bar *bar, Ball const *balls, int nbBalls) {
	float score = indesirableNumberOne(&balls[0]);
	float tmpScore;
	int indexBall = 0;
	float offset = 0;
	int i;

	for (i = 1; i < nbBalls; ++i) {
		tmpScore = indesirableNumberOne(&balls[i]);
		if (tmpScore > score && balls[i].respawnTimer == 0) {
			indexBall = i;
		}
	}
	offset = balls[indexBall].origin.x - bar->center.x;
	if (offset < -BAR_SPEED) {
		offset = -BAR_SPEED;
	}
	if (offset > BAR_SPEED) {
		offset = BAR_SPEED;
	}
	if (offset < 0) {
		if ((bar->center.x - (bar->width / 2)) >= (HUD_HEIGHT)) {
			bar->center.x += offset;
		}
	}
	if (offset > 0) {
		if ((bar->center.x + (bar->width / 2)) <= (SCREEN_WIDTH - HUD_HEIGHT)) {
			bar->center.x += offset;
		}
	}
}

int indesirableNumberOne(Ball const *ball) {
	if (ball->speed.y < 0) {
		return - ball->origin.y;
	} else {
		return ball->origin.y;
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
	if (brick->type == ADD_LIFE && players[ball->lastPlayerId - 1].life < 9) {
		++(players[ball->lastPlayerId - 1].life);
	}

	if (brick->type == SLOWER_BALL) {
		ball->speed.x = ball->speed.x < 0 ? - SLOW : SLOW;
		ball->speed.y = ball->speed.y < 0 ? - SLOW : SLOW;
	}
	if (brick->type == FASTER_BALL) {
		ball->speed.x = ball->speed.x < 0 ? - FAST : FAST;
		ball->speed.y = ball->speed.y < 0 ? - FAST : FAST;
	}
	if (brick->type == SLOWER_BALL || brick->type == FASTER_BALL) {
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
	ball->origin.x = SCREEN_WIDTH_CENTER;
	ball->respawnTimer = BALL_RESPAWN_TIME;
	ball->speed.y *= -1;
	ball->speed.x *= -1;
	if (dir == TOP) {
		--(players[0].life);
		ball->lastPlayerId = 1;
		ball->origin.y = HUD_HEIGHT + (3 * BAR_HEIGHT);
	}
	if (dir == BOTTOM) {
		--(players[1].life);
		ball->lastPlayerId = 2;
		ball->origin.y = SCREEN_HEIGHT - HUD_HEIGHT - (3 * BAR_HEIGHT);
	}
	if (dir == LEFT) {
		--(players[2].life);
		ball->lastPlayerId = 3;
		ball->origin.x = HUD_HEIGHT + (3 * BAR_HEIGHT);
	}
	if (dir == RIGHT) {
		--(players[3].life);
		ball->lastPlayerId = 4;
		ball->origin.x = SCREEN_WIDTH - HUD_HEIGHT - (3 * BAR_HEIGHT);
	}
}

/*/////////////////////////////////////////
 //		GAMEPLAY BRICK FUNCTIONS		//
/////////////////////////////////////////*/

/**
 * Define a brick color (TO DO TEXTURES) based on it's type.
 * @param	Brick	br	the current brick to look at
 */
int defineBrickColor(Brick br) {
	int indexTexture;
	if (br.type == 0) {
		indexTexture = ORDINARY;
	} else if (br.type == 1) {
		indexTexture = INDESTRUCTIBLE;
	} else if (br.type > 1 && (br.type % 2) == 0) {
		indexTexture = BONUS;
	} else {
		indexTexture = MALUS;
	}
	return indexTexture;
}
