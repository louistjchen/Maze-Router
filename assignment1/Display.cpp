/*
 * Display.cpp
 *
 *  Created on: Oct 3, 2018
 *      Author: chentuju
 */

#define SCBLOCK_SIZE		100.0
#define SCBLOCK_SIZE_HALF	40.0
#define LGBLOCK_SIZE		80.0
#define LGBLOCK_SIZE_HALF	40.0

#include "Display.h"


Router *pRouter = NULL;
int iterationNum = 0;

void openDisplay(Router *router) {

	pRouter = router;
	int gridSize = router->getGridSize();

	float xWindowSize = SCBLOCK_SIZE * (gridSize * 2 + 3);;
	float yWindowSize = SCBLOCK_SIZE * (gridSize * 2 + 3);;

    init_graphics("ECE1387 Assignment 1 - Maze Router (developed by Louis Chen)", WHITE);
    init_world (0.0, 0.0, xWindowSize, yWindowSize);
    update_message("Press \"Proceed\" to start maze router.");
    event_loop(act_on_button_press, NULL, NULL, drawScreen);
}

void updateDisplay() {

	create_button("Window", "Next Path", act_on_next_path_func);
	create_button("Next Path", "All Paths", act_on_all_paths_func);
    update_message("Finished maze routing. Press \"Next Path\" or \"All Paths\" to highlight path connection.");
	event_loop(act_on_button_press, NULL, NULL, updateScreen);
    close_graphics ();
}

void drawScreen() {

	set_draw_mode(DRAW_NORMAL);
	clearscreen();

	int gridSize = pRouter->getGridSize();
	int gridMax = gridSize*2+3;

	// draw all combined switch-connection blocks
	for(int i=1; i<gridMax; i+=2)
		for(int j=1; j<gridMax; j+=2)
			drawSCBlock(i*SCBLOCK_SIZE, j*SCBLOCK_SIZE, LIGHTGREY);

	// draw all logic blocks and associated 5 pins
	for(int i=3; i<gridMax; i+=2)
		for(int j=3; j<gridMax; j+=2) {
			drawLGBlock(i*SCBLOCK_SIZE, j*SCBLOCK_SIZE, DARKGREY,
					(i-3)/2, gridSize - (j-3)/2 - 1, WHITE);
			drawLGPin(i*SCBLOCK_SIZE, j*SCBLOCK_SIZE, BLACK, WHITE);
		}

	// draw all horizontal tracks
	for(int i=2; i<gridMax-1; i+=2)
		for(int j=2; j<gridMax; j+=2)
			drawHorizontalTrackSet(i*SCBLOCK_SIZE, j*SCBLOCK_SIZE, BLACK);

	// draw all vertical tracks
	for(int i=2; i<gridMax; i+=2)
		for(int j=2; j<gridMax-1; j+=2)
			drawVerticalTrackSet(i*SCBLOCK_SIZE, j*SCBLOCK_SIZE, BLACK);
}

void updateScreen() {

	drawScreen();

	// draw new paths
	std::vector<GoalPath*> *pGoalPath = pRouter->getGoalPath();
	int color = 4; 	// set available color (indicies 4-10)

//	for(int i=0; i<pGoalPath->size(); i++) {
	for(int i=0; i<iterationNum; i++) {
		GoalPath *goalPath = pGoalPath->at(i);

		// draw source target pins
		drawGoalPin(goalPath->getGoalPathInfo(1),
				goalPath->getGoalPathInfo(2),
				goalPath->getGoalPathInfo(3),
				color);
		drawGoalPin(goalPath->getGoalPathInfo(4),
				goalPath->getGoalPathInfo(5),
				goalPath->getGoalPathInfo(0),
				color);

		// draw track paths
		std::vector<Track*> *pPaths = goalPath->getPaths();
		for(unsigned int j=0; j<pPaths->size(); j++) {
			Track *track = pPaths->at(j);
			drawGoalTrack(track->getTrackInfo(1),
					track->getTrackInfo(2),
					track->getTrackInfo(3),
					track->getTrackInfo(4),
					color);
		}

		if(++color > 10)
			color = 4;
	}
}

void dontUpdateScreen() {
	flushinput();
}

void drawSCBlock(float x, float y, int color) {

	t_point scBlock[5] = {{0.0,SCBLOCK_SIZE_HALF},{SCBLOCK_SIZE_HALF,0.0},
			{SCBLOCK_SIZE,0.0},{SCBLOCK_SIZE,SCBLOCK_SIZE},
			{0.0,SCBLOCK_SIZE}};
	t_point temp[5];
	setcolor(color);
	for(int i=0; i<5; i++) {
		temp[i].x = x+scBlock[i].x;
		temp[i].y = y+scBlock[i].y;
	}
	fillpoly(temp, 5);
}

void drawLGBlock(float x, float y, int color, int xCoord, int yCoord, int coordColor) {

	t_point lgBlock[5] = {{-LGBLOCK_SIZE,-LGBLOCK_SIZE},{0.0,-LGBLOCK_SIZE},
			{0.0,-LGBLOCK_SIZE_HALF},{-LGBLOCK_SIZE_HALF,0.0},
			{-LGBLOCK_SIZE,0.0}};
	t_point temp[5];
	setcolor(color);
	for(int i=0; i<5; i++) {
		temp[i].x = x+lgBlock[i].x;
		temp[i].y = y+lgBlock[i].y;
	}
	fillpoly(temp, 5);
	setcolor(coordColor);
	setfontsize(15);
	std::ostringstream stm;
	stm << "(" << xCoord << "," << yCoord << ")";
	drawtext(x-LGBLOCK_SIZE_HALF-10, y-LGBLOCK_SIZE_HALF-20, stm.str().c_str(), LGBLOCK_SIZE);
}

void drawLGPin(float x, float y, int color, int numColor) {

	int numPin = 5;
	int delta = (SCBLOCK_SIZE_HALF)/numPin;
	int offset = delta/2;
	x = x - offset;
	y = y - (SCBLOCK_SIZE_HALF) + offset;
	setlinestyle(SOLID);
	setlinewidth(1);
	setfontsize(10);
	for(int i=0; i<numPin; i++) {
		setcolor(color);
		drawline(x-i*delta,
				y+i*delta,
				x-i*delta+SCBLOCK_SIZE_HALF,
				y+i*delta+SCBLOCK_SIZE_HALF);
		setcolor(numColor);
		char c = numPin - i + 48;
		char num[2];
		num[0] = c;
		num[1] = '\0';
		drawtext(x-i*delta-offset,
				y+i*delta-offset,
				num,
				LGBLOCK_SIZE);
	}
}

void drawHorizontalTrackSet(float x, float y, int color) {

	int trackWidth = pRouter->getTrackWidth();
	int delta = (SCBLOCK_SIZE - SCBLOCK_SIZE_HALF)/trackWidth;
	int offset = delta/2;
	setcolor(color);
	setlinestyle(SOLID);
	setlinewidth(1);
	setfontsize(10);
	for(int i=0; i<trackWidth; i++) {
		drawline(x, y-offset-i*delta, x+SCBLOCK_SIZE, y-offset-i*delta);
		char num[2];
		num[0] = i + 48;
		num[1] = '\0';
		drawtext(x+offset, y-offset-i*delta-offset, num, SCBLOCK_SIZE);
		drawtext(x+SCBLOCK_SIZE-offset, y-offset-i*delta-offset, num, SCBLOCK_SIZE);
	}
}

void drawVerticalTrackSet(float x, float y, int color) {

	int trackWidth = pRouter->getTrackWidth();
	int delta = (SCBLOCK_SIZE - SCBLOCK_SIZE_HALF)/trackWidth;
	int offset = delta/2;
	setcolor(color);
	setlinestyle(SOLID);
	setlinewidth(1);
	setfontsize(10);
	for(int i=0; i<trackWidth; i++) {
		drawline(x-offset-i*delta, y, x-offset-i*delta, y+SCBLOCK_SIZE);
		char num[2];
		num[0] = trackWidth - i + 47;
		num[1] = '\0';
		drawtext(x-i*delta, y+offset, num, SCBLOCK_SIZE);
		drawtext(x-i*delta, y+SCBLOCK_SIZE-offset, num, SCBLOCK_SIZE);
	}
}

void drawGoalPin(int x, int y, int pin, int color) {

	int gridSize = pRouter->getGridSize();
	x  = 3 + x * 2;
	y = 3 + (gridSize - y - 1) * 2;

	float xPos = x * SCBLOCK_SIZE;
	float yPos = y * SCBLOCK_SIZE;

	int numPin = 5;
	int delta = (SCBLOCK_SIZE_HALF)/numPin;
	int offset = delta/2;
	int i = numPin - pin;
	xPos = xPos - offset;
	yPos = yPos - (SCBLOCK_SIZE_HALF) + offset;
	setcolor(color);
	setlinestyle(SOLID);
	setlinewidth(3);
	drawline(xPos-i*delta,
			yPos+i*delta,
			xPos-i*delta+SCBLOCK_SIZE_HALF,
			yPos+i*delta+SCBLOCK_SIZE_HALF);
}

void drawGoalTrack(int alignType, int xCoord, int yCoord, int trackIndex, int color) {

	float xPos, yPos;
	int gridSize = pRouter->getGridSize();
	int trackWidth = pRouter->getTrackWidth();
	int delta = (SCBLOCK_SIZE - SCBLOCK_SIZE_HALF)/trackWidth;
	int offset = delta/2;
	int i;

	setcolor(color);
	setlinestyle(SOLID);
	setlinewidth(3);

	if(alignType == 0) {
		i = trackIndex;
		xPos = (xCoord * 2 + 2) * SCBLOCK_SIZE;
		yPos = ((gridSize - yCoord) * 2 + 2) * SCBLOCK_SIZE;
		drawline(xPos, yPos-offset-i*delta, xPos+SCBLOCK_SIZE, yPos-offset-i*delta);
	}
	else {
		i = trackWidth - trackIndex - 1;
		xPos = (xCoord * 2 + 2) * SCBLOCK_SIZE;
		yPos = ((gridSize - yCoord - 1) * 2 + 2) * SCBLOCK_SIZE;
		drawline(xPos-offset-i*delta, yPos, xPos-offset-i*delta, yPos+SCBLOCK_SIZE);
	}
}

void act_on_button_press(float x, float y) {
/* Called whenever event_loop gets a button press in the graphics *
 * area.  Allows the user to do whatever he/she wants with button *
 * clicks.                                                        */
//   printf("User clicked a button at coordinates (%f, %f)\n", x, y);
}

void act_on_mouse_move (float x, float y) {
/* function to handle mouse move event, the current mouse position  *
 * in the current world coordinate as defined as MAX_X and MAX_Y in *
 * init_world is returned.											*/
//   printf ("Mouse move at (%f,%f)\n", x, y);
}

void act_on_key_press (char c) {
/* function to handle keyboard press event, the ASCII character is returned */
//   printf ("Key press: %c\n", c);
}

void act_on_next_path_func (void (*drawscreen_ptr) (void)) {

	int goalPathSize = (int)pRouter->getGoalPath()->size();
	if(++iterationNum > goalPathSize)
		iterationNum = goalPathSize;
	updateScreen();
}

void act_on_all_paths_func (void (*drawscreen_ptr) (void)) {

	int goalPathSize = (int)pRouter->getGoalPath()->size();
	iterationNum = goalPathSize;
	updateScreen();
}
