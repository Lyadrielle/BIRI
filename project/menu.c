/**
 * @file		menu.c
 *       		menu functions library. Centralise all menu functions.
 * 			    from initialisations to drawing buttons or a menu.
 * 			    handling the buttons mouse or keyboard events.
 * @author	Calmels Gaëlle, Gallet Adrian
 * @version	1.0
 * @date		2017-05-07
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

#include "headers.h"

/*/////////////////////////////////////////
 //					MENU CONSTRUCTION						//
/////////////////////////////////////////*/

/**
 * Initialise a button based on the menu configuration. (vertical alignement always)
 * @param	Button*	bt			the current button pointer to be initialised
 * @param	Point2D	origin	the origin based on the menu configuration
 * @param	Color3f	color		the button color (TO DO TEXTURES)
 * @param	void		action	pointer on the action functions
 * @param	int			param		int parameter of the action function
 */
void initButton(Button *bt, Point2D origin, void (*action)(int), int param) {
	bt->origin.x = origin.x;
	bt->origin.y = origin.y;

	bt->color.r = 255;
	bt->color.g = 255;
	bt->color.b = 255;

	bt->action = action;
	bt->param = param;
}

/**
 * Initialise a menu based on an array of buttons.
 * @param	Button* menu	array of buttons
 */
void initMenu(Button *menu) {
	Point2D buttonOrigin;
	initPoint2D(&buttonOrigin, SCREEN_WIDTH_CENTER - (BUTTON_WIDTH / 2), 210);

	initButton(&menu[0], buttonOrigin, initGame, ONE_PL);

	buttonOrigin.y += BUTTON_HEIGHT + SPACE_BETWEEN_BUTTONS;
	initButton(&menu[1], buttonOrigin, initGame, TWO_PL);

	buttonOrigin.y += BUTTON_HEIGHT + SPACE_BETWEEN_BUTTONS;
	initButton(&menu[2], buttonOrigin, initGame, FOUR_PL);

	buttonOrigin.y += BUTTON_HEIGHT + SPACE_BETWEEN_BUTTONS;
	initButton(&menu[3], buttonOrigin, selectTheme, THEME1);

	buttonOrigin.y += BUTTON_HEIGHT + SPACE_BETWEEN_BUTTONS;
	initButton(&menu[4], buttonOrigin, quit, QUIT);
}

/**
 * Draw the menu of buttons.
 * @param	Button const*	menu	array of buttons (const)
 */
void drawMenu(Button const *menu) {
	int i;
	for (i = 0; i < NB_BUTTON_MAIN_MENU; ++i) {
		glEnable(GL_TEXTURE_2D);
		glColor3f(255, 255, 255);
		if (i == 3 && menu[i].param == THEME2) {
			glBindTexture(GL_TEXTURE_2D, texturesBuffer[12]);
		} else {
			glBindTexture(GL_TEXTURE_2D, texturesBuffer[7 + i]);
		}
		/*glColor3f(menu[i].color.r, menu[i].color.g, menu[i].color.b);*/
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(menu[i].origin.x, menu[i].origin.y);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f((menu[i].origin.x + BUTTON_WIDTH), menu[i].origin.y);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f((menu[i].origin.x + BUTTON_WIDTH), (menu[i].origin.y + BUTTON_HEIGHT));
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(menu[i].origin.x, (menu[i].origin.y + BUTTON_HEIGHT));
		glEnd();

		if (i == 3 && menu[i].param == THEME2) {
			glBindTexture(GL_TEXTURE_2D, texturesBuffer[12]);
		} else {
			glBindTexture(GL_TEXTURE_2D, texturesBuffer[7 + i]);
		}
		glDisable(GL_TEXTURE_2D);
	}
}

/*/////////////////////////////////////////
 //					MENU EVENT FUNCTIONS				//
/////////////////////////////////////////*/

/**
 * Handle one button events. Call it's action function.
 * @param	Button*		bt					the current button pointer
 * @param	SDL_Event	event				the trigger event to listen
 * @param	int*			currentStep	the current phase of the game
 */
int handleButton(Button *bt, SDL_Event event, int *currentStep) {
	Point2D clic;
	if ((event.type == SDL_MOUSEBUTTONUP) && (event.button.button == SDL_BUTTON_LEFT)) {
		clic.x = event.button.x;
		clic.y = event.button.y;
		if (isInsideButton(clic, *bt)) {
			if (bt->param < THEME1) {
				*currentStep = PLAYTIME;
			}
			if (bt->param > 9) {
				bt->param = bt->param == THEME1 ? THEME2 : THEME1;

				if (bt->param == THEME1) {
					initColor3f(&themeColor, 255, 139, 0);
				} else {
					initColor3f(&themeColor, 42, 239, 219);
				}
			}
			if (bt->param == QUIT) {
				*currentStep = QUIT_PROGRAM;
			}

			(*(bt->action))(bt->param);
			return bt->param;
		}
	}
	return 0;
}

/**
 * Verify if the mouse is inside the correct button action area.
 * @param		Point2D	clic	the current location of the mouse
 * @param		Button	bt		the current button to watch
 * @return	bool					return true if the clic is inside the action area
 */
bool isInsideButton(Point2D clic, Button bt) {
	return ((clic.x > bt.origin.x)
		&& (clic.y > bt.origin.y)
		&& (clic.x < (bt.origin.x + BUTTON_WIDTH))
		&& (clic.y < (bt.origin.y + BUTTON_HEIGHT))
	);
}

/**
 * Select the right theme and call for the loadTextures function
 * @param	inthemeId	the correct themePath
 */
void selectTheme(int themeId) {
	char path[100];
	sprintf(path, "img/THEME%d/", (themeId / 10));
	loadTextures(path);
}

/**
 * Just a function because all buttons need to call a function.
 * @param	int quit	quit
 */
void quit(int quit) {
	printf("QUIT !\n");
}
