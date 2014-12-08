/*
 * Main.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#include <vector>
#include <fstream>
#include <cstdlib>
#include <exception>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_message.H>

#include "Tile.h"
#include "Rulestring.h"

using namespace std;

// Default/Constant variables
const char App_Title[] = "Cellular Automata";
const char App_Version[] = "v0.0.0";
const int gw = 50;
const int gh = 50;
const int cw = 10;
const int ch = 10;
const int menuh = 30;
const int buttonh = 50;
const int shadefactor = 10;
const double timeout = 0.1;
const bool tutmode = false;

// Global variables
vector<Tile*> cells;
vector<int> currentRS = GameofLife_RS;

bool selectmode = false;
bool loadstampmode = false;
/*
 * TEMPDATA INFO
 *
 * tempdata[0]: x-coordinate for first corner
 * tempdata[1]: y-coordinate for first corner
 * tempdata[2]: x-coordinate for second corner
 * tempdata[3]: y-coordinate for second corner
 * tempdata[4]: index for the corners selected (to put coordinates in correct places)
 * tempdata[5]: width of stamp
 * tempdata[6]: height of stamp
 */
const int TD_FX = 0;	// Tempdata first x
const int TD_FY = 1;	// Tempdata first y
const int TD_SX = 2;	// Tempdata second x
const int TD_SY = 3;	// Tempdata second y
const int TD_CI = 4;	// Tempdata corner index
const int TD_SW = 5;	// Tempdata stamp width
const int TD_SH = 6;	// Tempdata stamp height
const int TD_STX = 7;	// Tempdata stamp x
const int TD_STY = 8;	// Tempdata stamp y
int tempdata[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};

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
void randtiles_cb(Fl_Widget*, void*);
void select_square_cb(Fl_Widget*, void*);
void corner_cb(Fl_Widget*, void*);
void inverttiles_cb(Fl_Widget*, void*);
void save_stamp_cb(Fl_Widget*, void*);
void load_stamp_cb(Fl_Widget*, void*);
void loadstamp_button_cb(Fl_Widget*, void*);
void createrule_cb(Fl_Widget*, void*);

// All the separate menu items
Fl_Menu_Item Menu_Items[] = {
	{"&File", 0, 0, 0, FL_SUBMENU},
		{"&Save Board", 0, save_board_cb},
		{"Save Stamp", 0, save_stamp_cb},
		{"&Load Board", 0, open_board_cb},
		{"Load Stamp", 0, load_stamp_cb, 0, FL_MENU_DIVIDER},
		{"E&xit", 0, quit_cb},
		{0},
	{"&Select", 0, 0, 0, FL_SUBMENU},
		{"A Square", 0, select_square_cb, 0, FL_MENU_DIVIDER},
		{"Drag Mode", 0, drag_cb, 0, FL_MENU_TOGGLE | FL_MENU_DIVIDER},
		{"Random Tiles", 0, randtiles_cb},
		{"Invert Tiles", 0, inverttiles_cb},
		{"Reset Tiles", 0, reset_cb},
		{0},
	{"Stamps", 0, 0, 0, FL_SUBMENU},
		{"Still Life", 0, 0, 0, FL_SUBMENU},
			{"Block", 0, load_stamp_cb, (void*)"stamps/block.ca"},
			{0},
		{"Oscillators", 0, 0, 0, FL_SUBMENU},
			{"Blinker", 0, load_stamp_cb, (void*)"stamps/blinker.ca"},
			{0},
		{"Spaceships", 0, 0, 0, FL_SUBMENU},
			{"Glider", 0, load_stamp_cb, (void*)"stamps/glider.ca"},
			{0},
		{0},
	{"&Rules", 0, 0, 0, FL_SUBMENU},
		{"Create Rule...", 0, createrule_cb, 0, FL_MENU_DIVIDER},
		{"Game of Life", 0, change_rule_cb, 0, FL_MENU_VALUE | FL_MENU_RADIO},
		{"HighLife", 0, change_rule_cb, (void*)1, FL_MENU_RADIO},
		{"Maze", 0, change_rule_cb, (void*)2, FL_MENU_RADIO},
		{"Mazectric", 0, change_rule_cb, (void*)3, FL_MENU_RADIO},
		{"Replicator", 0, change_rule_cb, (void*)4, FL_MENU_RADIO | FL_MENU_DIVIDER},
		{"Your own rule", 0, change_rule_cb, (void*)-1, FL_MENU_RADIO},
		{0},
	{"&Advanced", 0, 0, 0, FL_SUBMENU},
		{"Project...", 0, not_implemented},
		{0},
	{"&Help", 0, 0, 0, FL_SUBMENU},
		{"About Program", 0, not_implemented},
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
			t->setCoords(x, y);
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

	w->position(Fl::w()/2-w->w()/2, Fl::h()/2-w->h()/2);

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
						aroundtiles += int(cells[(y+ofy)*gw+x+ofx]->getState());
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
					if(aroundtiles == currentRS[i] && cells[y*gw+x]->getState())
						tempbool = true;
				}
				else if(negone)
				{
					// When it has looped through the middle separator
					// Do the rules for birth
					if(aroundtiles == currentRS[i] && !cells[y*gw+x]->getState())
						tempbool = true;
				}
				count++;
			}
			tempcells[y*gw+x] = tempbool;
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
	// Uses ASCII art to save
	// Asks user for filename
	char* fn = fl_file_chooser("Save Board", "Cellular Automata (*.ca)| All Files (*.*)", "/home/");
	if(fn == NULL)
	{
		// If user cancelled
		// return
		return;
	}

	// Turns the whole board into a string
	char wholeboard[gh*(gw+1)];
	for(int y=0; y<gh; y++)
	{
		for(int x=0; x<gw; x++)
		{
			if(cells[y*gw+x]->getState())
			{
				wholeboard[y*(gw+1)+x] = 'X';
			}
			else
				wholeboard[y*(gw+1)+x] = 'O';
		}
		wholeboard[y*(gw+1)+gw] = '\n';
	}

	// Write the whole thing to file
	try
	{
		ofstream file(fn);
		file << wholeboard;
		file.close();
	}
	catch(exception& ex)
	{
		msgbox(ex.what());
		return;
	}
}

void open_board_cb(Fl_Widget* w, void* data)
{
	// Asks user for filename
	char* fn = fl_file_chooser("Open Board", "Cellular Automata (*.ca)| All Files (*.*)", "/home/");
	if(fn == NULL)
	{
		// If user cancelled
		// return
		return;
	}

	// Read the file, delimited by '\n's
	int x=0;
	int y=0;
	bool err=false;
	bool skip=false;
	try
	{
		ifstream file(fn);
		char tmp;
		while(file.get(tmp))
		{
			// Out of range errors
			if(x >= gw)
			{
				err = true;
				skip = true;
				x=0;
			}
			if(y >= gh)
			{
				err = true;
				break;
			}

			if(tmp == '\n')
			{
				x=0;
				y++;
				skip = false;
				continue;
			}
			if(skip)
				continue;
			bool state = (tmp=='X'? true:false);
			if(cells[y*gw+x]->getState() != state)
			{
				cells[y*gw+x]->setState(state);
				cells[y*gw+x]->update_display();
			}
			x++;
		}
		file.close();
	}
	catch(exception& ex)
	{
		msgbox(ex.what());
		return;
	}

	// Error handling
	if(err)
	{
		// Gives out a warning
		msgbox("Warning: The file was probably not meant for a board of this size.");
	}
}

void quit_cb(Fl_Widget* w, void* data)
{
	exit(0);
}

void play_cb(Fl_Widget* w, void* data)
{
	if(!Fl::has_timeout(tick))
	{
		w->label("@square");
		w->tooltip("Stop the simulation");
		w->callback(stop_cb);
		Fl::add_timeout(timeout, tick);
	}
}

void stop_cb(Fl_Widget* w, void* data)
{
	if(Fl::has_timeout(tick))
	{
		w->label("@>");
		w->tooltip("Play/Start the simulation");
		w->callback(play_cb);
		Fl::remove_timeout(tick);
	}
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
	// If you are in select mode, only resets those that are selected
	if(selectmode && tempdata[TD_CI] == -1)
	{
		for(int y=tempdata[TD_FY]; y<=tempdata[TD_SY]; y++)
		{
			for(int x=tempdata[TD_FX]; x<=tempdata[TD_SX]; x++)
			{
				if(cells[y*gw+x]->getState())
				{
					cells[y*gw+x]->setState(false);
					cells[y*gw+x]->update_color();
					cells[y*gw+x]->color(cells[y*gw+x]->color() % shadefactor);
					cells[y*gw+x]->redraw();
				}
			}
		}
	}
	else
	{
		for(int i=0; i<cells.size(); i++)
		{
			if(cells[i]->getState())
			{
				// Update only if state = true
				cells[i]->setState(false);
				cells[i]->update_display();
			}
		}
	}
}

void randtiles_cb(Fl_Widget* w, void* data)
{
	// Generate a random grid of tiles
	// in selected area or whole board
	if(selectmode && tempdata[TD_CI] == -1)
	{
		for(int y=tempdata[TD_FY]; y<=tempdata[TD_SY]; y++)
		{
			for(int x=tempdata[TD_FX]; x<=tempdata[TD_SX]; x++)
			{
				bool state = (bool)(rand() % 2);
				if(cells[y*gw+x]->getState() != state)
				{
					cells[y*gw+x]->setState(state);
					cells[y*gw+x]->update_color();
					cells[y*gw+x]->color(cells[y*gw+x]->color() % shadefactor);
					cells[y*gw+x]->redraw();
				}
			}
		}
	}
	else
	{
		for(int i=0; i<cells.size(); i++)
		{
			bool state = (bool)(rand() % 2);
			if(cells[i]->getState() != state)
			{
				// Update when necessary
				cells[i]->setState(state);
				cells[i]->update_display();
			}
		}
	}
}

void select_square_cb(Fl_Widget* w, void* data)
{
	if(loadstampmode)
	{
		if(tutmode)
			msgbox("You are in load stamp mode. Cannot select.");
		return;
	}
	// Puts program into square select mode
	if(!selectmode)
	{
		// If program is not in select mode
		// Set up for select first corner
		// (Set the callbacks)
		for(int i=0; i<cells.size(); i++)
		{
			cells[i]->callback(corner_cb);
		}
		// Set selectmode
		selectmode = true;
		// Set index
		tempdata[TD_CI] = 0;
		// For tutorial mode
		if(tutmode)
			msgbox("Please select a corner.\nRight-click to cancel.");
	}
	else if(tempdata[TD_FX] != -1 && tempdata[TD_FY] != -1 && tempdata[TD_SX] != -1 &&
			tempdata[TD_SY] != -1 && tempdata[TD_CI] != -1 && selectmode)
	{
		// Test all the points first
		// Rearrange if neccessary
		if(tempdata[TD_FX] > tempdata[TD_SX])
		{
			int temp = tempdata[TD_FX];
			tempdata[TD_FX] = tempdata[TD_SX];
			tempdata[TD_SX] = temp;
		}
		if(tempdata[TD_FY] > tempdata[TD_SY])
		{
			int temp = tempdata[TD_FY];
			tempdata[TD_FY] = tempdata[TD_SY];
			tempdata[TD_SY] = temp;
		}
		// If you are done inputting the 2 corners,
		// put the stuff back
		// Reset all displays first
		for(int i=0; i<cells.size(); i++)
		{
			cells[i]->update_display();
		}
		for(int y=tempdata[TD_FY]; y<=tempdata[TD_SY]; y++)
		{
			for(int x=tempdata[TD_FX]; x<=tempdata[TD_SX]; x++)
			{
				// Alternate coloring for the area
				cells[y*gw+x]->color(cells[y*gw+x]->color() % shadefactor);
				cells[y*gw+x]->redraw();
			}
		}
		// Reset the index
		tempdata[TD_CI] = -1;
	}
	else if(tempdata[TD_FX] == -2)
	{
		// If you right-click (cancel operation)
		// reset buttons
		for(int i=0; i<cells.size(); i++)
		{
			cells[i]->callback(tile_cb);
			cells[i]->update_display();
		}
		selectmode = false;
		// Reset the temporary data
		for(int i=0; i<sizeof(tempdata)/sizeof(int); i++)
			tempdata[i] = -1;
	}
}

void corner_cb(Fl_Widget* w, void* data)
{
	// Check for right-click. If there is right-click, cancel select.
	if(Fl::event_button() == FL_RIGHT_MOUSE)
	{
		// Set to cancel
		tempdata[TD_FX] = -2;
	}
	else
	{
		Tile* t = (Tile*)w;
		// Set the index
		if(tempdata[TD_CI] == -1)
		{
			tempdata[TD_CI] = 0;
			// To allow selecting and selecting again
			tempdata[TD_SY] = -1;
		}
		// Set the corners
		tempdata[tempdata[TD_CI]] = t->getX();
		tempdata[tempdata[TD_CI]+1] = t->getY();
		tempdata[TD_CI] += 2;							// Increment index
		// Colour it in
		t->color(t->color() % shadefactor);
		t->redraw();
		// Tutorial mode
		if(tutmode && tempdata[TD_SY] == -1)
			msgbox("Please select another corner.\nRight-click to cancel.");
	}
	select_square_cb(w, data);
}

void inverttiles_cb(Fl_Widget* w, void* data)
{
	// Inverts all the tiles, if none are selected
	// You have to be in select mode. (Obviously)
	if(selectmode && tempdata[TD_CI] == -1)
	{
		for(int y=tempdata[TD_FY]; y<=tempdata[TD_SY]; y++)
		{
			for(int x=tempdata[TD_FX]; x<=tempdata[TD_SX]; x++)
			{
				cells[y*gw+x]->setState(!cells[y*gw+x]->getState());
				cells[y*gw+x]->update_color();
				cells[y*gw+x]->color(cells[y*gw+x]->color() % shadefactor);
				cells[y*gw+x]->redraw();
			}
		}
	}
	else
	{
		for(int i=0; i<cells.size(); i++)
		{
			// Unfortunately, this has you updating
			// all the tiles, so I can't see any
			// optimizations to implement.
			cells[i]->setState(!cells[i]->getState());
			cells[i]->update_display();
		}
	}
}

void save_stamp_cb(Fl_Widget* w, void* data)
{
	// Make sure that you have selected an area
	if(selectmode && tempdata[TD_CI] == -1)
	{
		// Ask for filename
		char* fn = fl_file_chooser("Save Stamp...", "Cellular Automata (*.ca)|All Files (*.*)", ".");
		if(fn == NULL)
			return;

		int sw = tempdata[TD_SX]-tempdata[TD_FX];
		int sh = tempdata[TD_SY]-tempdata[TD_FY];
		int i=0;
		char* stamp = new char[(sw+2)*(sh+1)];
		// Create the stamp to write to file
		for(int y=tempdata[TD_FY]; y<=tempdata[TD_SY]; y++)
		{
			for(int x=tempdata[TD_FX]; x<=tempdata[TD_SX]+1; x++)
			{
				char pc = 'O';
				if(x-tempdata[TD_FX] == sw+1)
				{
					pc = '\n';
				}
				else if(cells[y*gw+x]->getState())
				{
					pc = 'X';
				}
				stamp[i++] = pc;
			}
		}
		try
		{
			ofstream f(fn);
			f << stamp;
			f.close();
		}
		catch(exception& ex)
		{
			msgbox(ex.what());
		}
	}
	else
	{
		if(tutmode)
			msgbox("Please select the area first.");
	}
}

void load_stamp_cb(Fl_Widget* w, void* data)
{
	if(selectmode)
	{
		if(tutmode)
			msgbox("You are in select mode. Cannot load stamp.");
		return;
	}
	if(tempdata[TD_FX] == -2)
	{
		if(tutmode)
			msgbox("Cancelled");
		// Cancel
		for(int i=0; i<sizeof(tempdata)/sizeof(int); i++)
			tempdata[i] = -1;
		// Callback reset
		for(int i=0; i<cells.size(); i++)
			cells[i]->callback(tile_cb);
		return;
	}
	char* loadfn = (char*)data;
	if(loadfn == 0)
	{
		// Get the file
		loadfn = fl_file_chooser("Load Stamp...", "Cellular Automata (*.ca)|All Files (*.*)", ".");
		if(loadfn == NULL)
			return;
	}

	// Assuming we've got the file, open and read it
	vector<char> filestring;
	int sw = 0;
	int sh = 0;
	try
	{
		ifstream f(loadfn);
		char c;
		while(f.get(c))
		{
			filestring.push_back(c);
			if(c == '\n')
				sh++;
			else
				sw++;
		}
		tempdata[TD_SW] = sw/sh;
		tempdata[TD_SH] = sh;
		f.close();
	}
	catch(exception& ex)
	{
		msgbox(ex.what());
	}

	// Check for load stamp mode
	if(loadstampmode)
	{
		// If it is directed here from a button
		int i=0;
		for(int y=tempdata[TD_STY]; y<tempdata[TD_STY]+tempdata[TD_SH]; y++)
		{
			for(int x=tempdata[TD_STX]; x<tempdata[TD_STX]+tempdata[TD_SW]; x++)
			{
				cells[y*gw+x]->setState((filestring[i++] == 'X'? true:false));
				cells[y*gw+x]->update_display();

				if(filestring[i] == '\n')
					i++;
			}
		}

		// Reset everything
		for(int i=0; i<sizeof(tempdata)/sizeof(int); i++)
			tempdata[i] = -1;
		// Callback reset
		for(int i=0; i<cells.size(); i++)
			cells[i]->callback(tile_cb);
		loadstampmode = false;
	}
	else
	{
		// Change all button callbacks
		for(int i=0; i<cells.size(); i++)
		{
			cells[i]->callback(loadstamp_button_cb, (void*)loadfn);
		}
		loadstampmode = true;
	}
}

void loadstamp_button_cb(Fl_Widget* w, void* data)
{
	// Check for right-click. If there is right-click, cancel select.
	if(Fl::event_button() == FL_RIGHT_MOUSE)
	{
		// Set to cancel
		tempdata[TD_FX] = -2;
	}
	else
	{
		Tile* t = (Tile*)w;
		tempdata[TD_STX] = t->getX();
		tempdata[TD_STY] = t->getY();
	}
	load_stamp_cb(w, data);
}

void createrule_cb(Fl_Widget* w, void* data)
{
	// Ask for rulestring
	char* rs = (char*)fl_input("Please enter you rulestring (Survival/Reproduction)\nExample\nGame of Life 23/3");
	if(rs == NULL)
		return;

	// Parse it down
	for(int i=0; rs[i] != '\0'; i++)
	{
		if(rs[i] == '/')
			Own_RS.push_back(-1);
		else if(rs[i] >= '0' && rs[i] <= '8')
			Own_RS.push_back(static_cast<int>(rs[i]-'0'));
		else
			msgbox("Error: Invalid number in rulestring.");
	}

	// Tell user
	if(tutmode)
		msgbox("Rulestring has been successfully parsed into program. Please select it in menu");
}
