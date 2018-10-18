/*
 * Display.h
 *
 *  Created on: Oct 3, 2018
 *      Author: chentuju
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include "graphics.h"
#include "Router.h"

// Functions for main.cpp to invoke
void openDisplay(Router *router);
void updateDisplay();

// Callbacks for event-driven window handling.
void drawScreen();
void updateScreen();
void act_on_button_press(float x, float y);
void act_on_mouse_move(float x, float y);
void act_on_key_press(char c);
void act_on_next_path_func(void (*drawscreen_ptr) (void));
void act_on_all_paths_func(void (*drawscreen_ptr) (void));

// Helper functions
void drawSCBlock(float x, float y, int color);
void drawLGBlock(float x, float y, int color, int xCoord, int yCoord, int coordColor);
void drawLGPin(float x, float y, int color, int numColor);
void drawHorizontalTrackSet(float x, float y, int color);
void drawVerticalTrackSet(float x, float y, int color);
void drawGoalPin(int x, int y, int pin, int color);
void drawGoalTrack(int alignType, int xCoord, int yCoord, int trackIndex, int color);

#endif /* DISPLAY_H_ */
