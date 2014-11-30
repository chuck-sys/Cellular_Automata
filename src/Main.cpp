/*
 * Main.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/fl_message.H>

#include "Tile.h"

using namespace std;

// Default/Constant variables
const char* App_Title = "Cellular Automata";
const int gw = 10;
const int gh = 10;
const int cw = 30;
const int ch = 30;
const int menuh = 30;

// Global variables
vector<Tile*> cells;

// Function prototypes
void msgbox(const char*, const char*);

void not_implemented(Fl_Widget*, void*);
void tile_cb(Fl_Widget*, void*);
void save_board_cb(Fl_Widget*, void*);
void open_board_cb(Fl_Widget*, void*);
void quit_cb(Fl_Widget*, void*);

// All the separate menu items
Fl_Menu_Item Menu_Items[] = {
	{"&File", 0, 0, 0, FL_SUBMENU},
		{"&Save Board", 0, not_implemented},
		{"Save Stamp", 0, not_implemented},
		{"&Load Board", 0, not_implemented},
		{"Load Stamp", 0, not_implemented, 0, FL_MENU_DIVIDER},
		{"E&xit", 0, quit_cb},
		{0},
	{"&Select", 0, 0, 0, FL_SUBMENU},
		{"Drag Mode", 0, not_implemented, 0, FL_MENU_TOGGLE | FL_MENU_VALUE | FL_MENU_DIVIDER},
		{"Random Tiles", 0, not_implemented},
		{"Reset Tiles", 0, not_implemented},
		{0},
	{"Stamps", 0, 0, 0, FL_SUBMENU},
		{"Still Life", 0, 0, 0, FL_SUBMENU},
			{0},
		{"Oscillators", 0, 0, 0, FL_SUBMENU},
			{0},
		{"Spaceships", 0, 0, 0, FL_SUBMENU},
			{0},
		{0},
	{"&Rules", 0, 0, 0, FL_SUBMENU},
		{"Game of Life", 0, not_implemented, 0, FL_MENU_VALUE | FL_MENU_RADIO},
		{"HighLife", 0, not_implemented, 0, FL_MENU_RADIO},
		{"Maze", 0, not_implemented, 0, FL_MENU_RADIO},
		{0},
};

int main(int argc, char* argv[])
{
	Fl_Double_Window* w = new Fl_Double_Window(gw*cw, gh*ch+menuh, App_Title);
	w->begin();

	// Add all the cells in
	for(int y=0; y<gh; y++)
	{
		for(int x=0; x<gw; x++)
		{
			Tile* t = new Tile(x*cw, y*ch+menuh, cw, ch);
			t->callback(tile_cb);
			cells.push_back(t);
		}
	}

	// Now for the menubar
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, w->w(), menuh);

	w->end();

	menu->copy(Menu_Items);

	w->show(argc, argv);

	return Fl::run();
}

void msgbox(const char* msg, const char* title=App_Title)
{
	fl_message_title(title);
	fl_message(msg);
}

void not_implemented(Fl_Widget* w, void* data)
{
	msgbox("This feature has not beeen implemented yet.", "Sorry :-(");
}

void tile_cb(Fl_Widget* w, void* data)
{
	Tile* tw = (Tile*)w;
	tw->toggle();
	tw->update_display();
}

void save_board_cb(Fl_Widget* w, void* data)
{

}

void open_board_cb(Fl_Widget* w, void* data)
{

}

void quit_cb(Fl_Widget* w, void* data)
{
	exit(0);
}

