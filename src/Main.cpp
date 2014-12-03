/*
 * Main.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#include <vector>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/fl_message.H>

#include "Tile.h"
#include "Rulestring.h"

using namespace std;

// Default/Constant variables
const char* App_Title = "Cellular Automata";
const int gw = 50;
const int gh = 50;
const int cw = 10;
const int ch = 10;
const int menuh = 30;
const int buttonh = 50;
const double timeout = 0.05;

// Global variables
vector<Tile*> cells;
vector<int> currentRS = GameofLife_RS;

// Function prototypes
void msgbox(const char*, const char*);
void tick(void*);

void not_implemented(Fl_Widget*, void*);
void tile_cb(Fl_Widget*, void*);
void save_board_cb(Fl_Widget*, void*);
void open_board_cb(Fl_Widget*, void*);
void quit_cb(Fl_Widget*, void*);
void play_cb(Fl_Widget*, void*);
void stop_cb(Fl_Widget*, void*);
void step_cb(Fl_Widget*, void*);
void drag_cb(Fl_Widget*, void*);
void change_rule_cb(Fl_Widget*, void*);
void reset_cb(Fl_Widget*, void*);

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
		{"Drag Mode", 0, drag_cb, 0, FL_MENU_TOGGLE | FL_MENU_DIVIDER},
		{"Random Tiles", 0, not_implemented},
		{"Reset Tiles", 0, reset_cb},
		{0},
	{"Stamps", 0, 0, 0, FL_SUBMENU},
		{"Still Life", 0, 0, 0, FL_SUBMENU},
			{"Block", 0, not_implemented},
			{0},
		{"Oscillators", 0, 0, 0, FL_SUBMENU},
			{"Blinker", 0, not_implemented},
			{0},
		{"Spaceships", 0, 0, 0, FL_SUBMENU},
			{"Glider", 0, not_implemented},
			{0},
		{0},
	{"&Rules", 0, 0, 0, FL_SUBMENU},
		{"Create Rule...", 0, not_implemented, 0, FL_MENU_DIVIDER},
		{"Game of Life", 0, change_rule_cb, 0, FL_MENU_VALUE | FL_MENU_RADIO},
		{"HighLife", 0, change_rule_cb, (void*)1, FL_MENU_RADIO},
		{"Maze", 0, change_rule_cb, (void*)2, FL_MENU_RADIO},
		{"Mazectric", 0, change_rule_cb, (void*)3, FL_MENU_RADIO},
		{"Replicator", 0, change_rule_cb, (void*)4, FL_MENU_RADIO | FL_MENU_DIVIDER},
		{"Your own rule", 0, change_rule_cb, (void*)-1, FL_MENU_RADIO},
		{0},
	{0}
};

int main(int argc, char* argv[])
{
	Fl_Double_Window* w = new Fl_Double_Window(gw*cw, gh*ch+menuh+buttonh, App_Title);
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
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, w->w()+1, menuh);

	// The play button
	Fl_Button* play_bt = new Fl_Button(0, ch*gh+menuh, w->w()/2, buttonh, "@>");
	// The step button
	Fl_Button* step_bt = new Fl_Button(w->w()/2, ch*gh+menuh, w->w()/2, buttonh, "@->|");

	w->end();

	menu->copy(Menu_Items);
	play_bt->callback(play_cb);
	step_bt->callback(step_cb);

	play_bt->tooltip("Play/Start the simulation");
	step_bt->tooltip("Step through simulation by one");

	w->show(argc, argv);

	return Fl::run();
}

void msgbox(const char* msg, const char* title=App_Title)
{
	fl_message_title(title);
	fl_message(msg);
}

void tick(void* step)
{
	bool tempcells[gw*gh];				// Temporary array
	for(int y=0; y<gh; y++)
	{
		for(int x=0; x<gw; x++)
		{
			int aroundtiles = 0;
			// The offsets
			for(short ofy=-1; ofy<2; ofy++)
			{
				for(short ofx=-1; ofx<2; ofx++)
				{
					if(((x+ofx>=0) && (y+ofy>=0) && (x+ofx < gw) && (y+ofy < gh) && !((ofx == 0) && (ofy == 0))))
					{
						aroundtiles += int(cells[(y+ofy)*gh+x+ofx]->getState());
					}
				}
			}
			bool tempbool = false;		// False by default

			// Now comes the tricky bit: Looping over the rules
			bool negone = false;
			int count = 0;
			for(int i=0; i<currentRS.size(); i++)
			{
				if(currentRS[i] == -1)
				{
					negone = true;
				}
				else if(!negone)
				{
					// When it hasn't looped through middle separator yet
					// Do the rules for survival
					if(aroundtiles == currentRS[i] && cells[y*gh+x]->getState())
						tempbool = true;
				}
				else if(negone)
				{
					// When it has looped through the middle separator
					// Do the rules for birth
					if(aroundtiles == currentRS[i] && !cells[y*gh+x]->getState())
						tempbool = true;
				}
				count++;
			}
			tempcells[y*gh+x] = tempbool;
		}
	}

	// Put tempcells into cells
	for(int i=0; i<sizeof(tempcells)/sizeof(bool); i++)
	{
		if(cells[i]->getState() != tempcells[i])
		{
			// Update only if necessary
			cells[i]->setState(tempcells[i]);
			cells[i]->update_display();
		}
	}

	if(!(bool)step)
		Fl::repeat_timeout(timeout, tick);
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

void play_cb(Fl_Widget* w, void* data)
{
	w->label("@square");
	w->tooltip("Stop the simulation");
	w->callback(stop_cb);
	Fl::add_timeout(timeout, tick);
}

void stop_cb(Fl_Widget* w, void* data)
{
	w->label("@>");
	w->tooltip("Play/Start the simulation");
	w->callback(play_cb);
	Fl::remove_timeout(tick);
}

void step_cb(Fl_Widget* w, void* data)
{
	tick((void*)true);
}

void drag_cb(Fl_Widget* w, void* data)
{
	Tile::DragMode = !Tile::DragMode;
}

void change_rule_cb(Fl_Widget* w, void* data)
{
	switch((long)data)
	{
	case -1:
		// #-1	Your own rule
		if(Own_RS.size() == 0)
		{
			// If you haven't established your own ruleset, just ask
			not_implemented(w, data);
		}
		currentRS = Own_RS;
		break;
	case 0:
		// #0	Game of Life
		currentRS = GameofLife_RS;
		break;
	case 1:
		// #1	HighLife
		currentRS = HighLife_RS;
		break;
	case 2:
		// #2	Maze
		currentRS = Maze_RS;
		break;
	case 3:
		// #3	Mazectric
		currentRS = Mazectric_RS;
		break;
	case 4:
		// #4	Replicator
		currentRS = Replicator_RS;
		break;
	default:
		// Default is 0 (Game of Life)
		currentRS = GameofLife_RS;
		break;
	}
}

void reset_cb(Fl_Widget* w, void* data)
{
	// Resets the tiles
	for(int i=0; i<gw*gh; i++)
	{
		if(cells[i]->getState())
		{
			// Update only if state = true
			cells[i]->setState(false);
			cells[i]->update_display();
		}
	}
}

