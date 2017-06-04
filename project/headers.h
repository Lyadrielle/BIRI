/**
 * @file	header.h
 *       	List all the program functions : enumerations, structures, unions....
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date	2017-04-23
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_image.h>

 /* TEXTURES */

/*/////////////////////////////////////////
 //			CONSTANTS DEFINITION		//
/////////////////////////////////////////*/

/* ----------( SCREEN )---------- */
#define SCREEN_WIDTH 1080
#define SCREEN_WIDTH_CENTER (SCREEN_WIDTH / 2)
#define SCREEN_HEIGHT 700
#define SCREEN_HEIGHT_CENTER (SCREEN_HEIGHT / 2)
#define HUD_HEIGHT 70
#define GAME_HEIGHT (SCREEN_HEIGHT - (2 * HUD_HEIGHT))
#define GAME_WIDTH (SCREEN_WIDTH - (2 * HUD_HEIGHT))
#define BIT_PER_PIXEL 32
#define TEXTURE_NB 16

/* -----------( MENU )----------- */
#define NB_BUTTON_MAIN_MENU 5
#define BUTTON_WIDTH 360
#define BUTTON_HEIGHT 60
#define SPACE_BETWEEN_BUTTONS 20

/* -----------( BRICK )---------- */
#define BRICK_WIDTH 62
#define BRICK_HEIGHT 32
/* -----------( BAR )------------ */
#define BAR_HEIGHT 12
#define BAR_SPEED 6

/* -----------( BALL )------------ */
#define BALL_RADIUS 7

/* ---------( GAMEPLAY )--------- */
#define START_LIFE 3
#define BALL_RESPAWN_TIME 100
#define BALL_BONUS_TIME 600

/* -----------( MATHS )---------- */
#define PI 3.1415926535897932384626433832795

/* -----------( OTHER )---------- */
#define MALLOC_ERROR -3
#define LIFE_SIZE_WIDTH 20
#define LIFE_SIZE_HEIGHT 26

/*/////////////////////////////////////////
 //			ENUMERATIONS				//
/////////////////////////////////////////*/

enum difficulty {
	EASY = 1,
	MEDIUM = 2,
	HARD = 3
};

enum speed {
	SLOW = 2,
	NORMAL = 3,
	FAST = 4
};

enum barSize {
	SMALL = 100,
	BASIC = 140,
	LARGE = 180
};

enum brickStatus {
	PRISTINE,
	DAMAGED,
	DESTROYED
};

enum brickType {
	ORDINARY = 0,
	INDESTRUCTIBLE = 1,
	WIDER_BAR = 2,
	SMALLER_BAR = 3,
	ADD_LIFE = 4,
	FASTER_BALL = 5,
	SLOWER_BALL = 6
};

enum brickTexture {
	BONUS = 2,
	MALUS = 3
};

enum direction {
	NONE,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT
};

enum collisionType {
	NO_COLLISION = 0,
	SEGMENT,
	CORNER_A,
	CORNER_B
};

enum gameMode {
	ONE_PL = 1,
	TWO_PL = 2,
	TRHEE_PL = 3,
	FOUR_PL = 4,
	QUIT = 5,
	THEME1 = 10,
	THEME2 = 20
};

enum gameStep {
	INITIALISATON,
	PLAYTIME,
	SCOREBOARD,
	QUIT_PROGRAM,
	PAUSE
};

/*/////////////////////////////////////////
 //			GEOMETRIC STRUCTURES		//
/////////////////////////////////////////*/

typedef struct Point2D {
	float x, y;
} Point2D;

typedef struct Vector2D {
	float x, y;
} Vector2D;

typedef struct Color3f {
	float r, g, b;
} Color3f;

/*/////////////////////////////////////////
 //			GAME STRUCTURES				//
/////////////////////////////////////////*/

typedef struct Ball {
	int id;
	int radius;
	int respawnTimer;
	int bonusTimer;
	Point2D origin;
	Vector2D speed;
	Color3f color;
	int lastPlayerId;
} Ball;

typedef struct Bar {
	int playerId;
	int width;
	Point2D center;
	Color3f color;
	bool orientationHorizontal;
} Bar;

typedef struct Brick {
	int type;
	int status;
	int gridX;
	int gridY;

	Point2D topLeft;
	Point2D topRight;
	Point2D bottomLeft;
	Point2D bottomRight;
} Brick;

typedef Brick** GridBrick;

/*/////////////////////////////////////////
 //			GAMEPLAY STRUCTURES			//
/////////////////////////////////////////*/

typedef struct Player {
	int id;
	char *name;
	Bar bar;
	int life;
	bool immune;
	int score;
} Player;

/*/////////////////////////////////////////
 //			MENU STRUCTURES				//
/////////////////////////////////////////*/

typedef struct Button {
	Point2D origin;
	Color3f color;
	void (*action)(int);
	int param;
} Button;

typedef struct TextField {
	Point2D origin;
	Color3f color;
	void (*action)(char*);
	char *name;
} TextField;

/*/////////////////////////////////////////
 //			GLOBAL VARIABLES DEF		//
/////////////////////////////////////////*/

extern int screenWidth;
extern int screenWidthCenter;
extern Ball *balls;
extern Player *players;
extern char *playersNames[];
extern GLuint texturesBuffer[];
extern Color3f themeColor;

/*/////////////////////////////////////////
 //			FUNCTIONS PROTOTYPE			//
/////////////////////////////////////////*/

/* ------------( core.c )------------ */

int *readConfigFile(char *filePath, int *gridWidth, int *gridHeight);

int defineScreenWidth(int gridWidth);
int defineBrickWidth(int gridWidth);

void initBar(Bar *bar, Point2D center, Color3f color, int playerId);
void initBall(Ball *bl, int id, int radius, Vector2D speed, Point2D origin, Color3f color, int lastPlayerId);
void initBrick(Brick *b, int type, enum brickStatus status, int indexX, int indexY);
void updateBrickCoordinates(Brick *br, Point2D topLeft);
void initBrickCoordinates(GridBrick grid, int gridWidth, int gridHeight);
GridBrick initGrid(int gridWidth, int gridHeight, int *blockType);

/* ----------( geometry.c )---------- */

void initPoint2D(Point2D *p, float x, float y);
void initColor3f(Color3f *c, float r, float g, float b);
void initVector2D(Vector2D *v, float x, float y);
Vector2D defineVector(Point2D a, Point2D b);

Point2D pointPlusVector( Point2D p, Vector2D v);
Vector2D addVectors(Vector2D vA, Vector2D vB);
Vector2D subVectors(Vector2D vA, Vector2D vB);
Vector2D multVector(Vector2D v, float a);
Vector2D divVector(Vector2D v, float a);

float dotPRoduct(Vector2D vA, Vector2D vB);
float norm(Vector2D v);
Vector2D normalize(Vector2D v);
float distance(Point2D a, Point2D b);

/* ------------( collision.c )----------- */

void collisionBallScreen(Ball *ball, int nbPlayers);
bool collisionBallLine(Ball const *ball, Point2D A, Point2D B);
enum collisionType collisionBallSegment(Ball const *ball, Point2D A, Point2D B);
enum direction collisionBallBrick(Ball const *ball, Brick const *brick);
bool collisionBallGrid(GridBrick grid, Ball *ball, int gridWidth, int gridHeight);

void collisionBarBall(Bar const *bar, Ball *ball);

void moveBall(Ball *ball);

/* ------------( display.c )----------- */

void drawBar(Bar bar);
void drawBall(Ball ball);
void drawBrick(Brick br);
void drawGrid(GridBrick const grid,int gridWidth, int gridHeight);
void drawBackground(int index);
void drawHUD(Player const *pl, int nbPlayers);
void drawLife();
void drawLifes(int nbHearts);
void renderBitmapString(float x, float y, char const *string);
void renderBitmapVerticalString(float x, float y, char const *string);
void printVictoryScreen(Player const *players, int nbPlayers, bool gladOS);
int NameLenght(char const *name);

/* TEXTURE */
GLenum testFormat(SDL_Surface *img);
void loadTextures(char *themePath);
void chargeTexture(char *imgaddress);


/* ----------( gameplay.c )---------- */

void initPlayer(Player *pl, int id, char *name, Point2D barCenter, Color3f barColor);
void initGame(int nbPlayers);

void moveBar (Bar *bar, enum direction dir);
void handleGladOS(Bar *bar, Ball const *balls, int nbBalls);
int indesirableNumberOne(Ball const *ball);
void hitBrick (Brick *brick, Ball *ball);
void ballOutOfBounds(Ball *ball, enum direction dir);

int defineBrickColor(Brick br);

/* ----------( menu.c )---------- */

void initButton(Button *bt, Point2D origin, void (*action)(int), int param);
void initMenu(Button *menu);
void drawMenu(Button const *menu);

int handleButton(Button *bt, SDL_Event event, int *currentStep);
bool isInsideButton(Point2D clic, Button bt);
void selectTheme(int themeId);
void quit(int quit);
