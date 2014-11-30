/*
 * Tile.h
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#ifndef SRC_TILE_H_
#define SRC_TILE_H_

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

class Tile: public Fl_Button
{
private:
	bool state;

public:
	static int AliveCol;
	static int DeadCol;

	Tile(int x, int y, int w, int h, char* l=0);

	int handle(int);				// Overrides the default event handler
	void draw();					// Overrides the default draw function
	void update_display();

	void toggle() {this->state = !this->state;};
};

#endif /* SRC_TILE_H_ */
