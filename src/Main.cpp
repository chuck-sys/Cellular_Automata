/*
 * Main.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_message.H>

#include "Tile.h"
#include "Rulestring.h"
#include "LuaHelper.h"

using namespace std;

// Default/Constant variables
const char App_Title[] = "Cellular Automata";
const char App_Version[] = "v1.0.0";
const char App_Info[] = "Cellular Automata is a program which lets you experiment with\n"
		"different types of cellular automata, hence the name.\n"
		"By playing around with this program, you get to find out more about\n"
		"how a certain cellular automata works, or perhaps invent your own.";
int txteditw = 300;		// Width of text editor
int gw = 50;					// Grid width
int gh = 50;					// Grid height
int cw = 10;					// Cell width (px)
int ch = 10;					// Cell height (px)
int menuh = 30;					// Menubar height (px)
int buttonh = 50;				// Button height (px)
int shadefactor = 10;			// The Shade factor (for shading out selected squares
double timeout = 0.1;			// The timeout between each tick (s)
bool tutmode = false;			// Whether we are in tutorial mode or not

// Global variables
Lua_Helper lh;

vector<Tile*> cells;
vector<int> currentRS = GameofLife_RS;

bool extendedmode = false;
bool selectmode = false;
bool loadstampmode = false;
bool projectmode = false;
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

// Global FL variables
Fl_Menu_Bar* menu;
Fl_Text_Buffer* scriptbuf;
Fl_Double_Window* window;

// Function prototypes
void msgbox(const char*, const char*);
void tick(void*);
void get_config(Lua_Helper);
int abs(int);
void rs_init();
void lua_funcinit();

int lua_createrule(lua_State*);
int lua_rgb_col(lua_State*);
int lua_msgbox(lua_State*);
int lua_toggle(lua_State*);

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
void about_cb(Fl_Widget*, void*);
void project_cb(Fl_Widget*, void*);
void toggle_console_cb(Fl_Widget*, void*);
void run_lua_script_cb(Fl_Widget*, void*);
void os_script_cb(Fl_Widget*, void*);

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
		{"Game of Life", 0, change_rule_cb, (void*)0, FL_MENU_VALUE | FL_MENU_RADIO},
		{"HighLife", 0, change_rule_cb, (void*)1, FL_MENU_RADIO},
		{"Maze", 0, change_rule_cb, (void*)2, FL_MENU_RADIO},
		{"Mazectric", 0, change_rule_cb, (void*)3, FL_MENU_RADIO},
		{"Replicator", 0, change_rule_cb, (void*)4, FL_MENU_RADIO},
		{0},
	{"&Advanced", 0, 0, 0, FL_SUBMENU},
		{"Project...", 0, project_cb, 0, FL_MENU_DIVIDER},
		{"Toggle Lua Console", 0, toggle_console_cb},
		{"Open Script", 0, os_script_cb, 0},
		{"Save Script", 0, os_script_cb, (void*)1},
		{"Run Script", 0, run_lua_script_cb},
		{0},
	{"&Help", 0, 0, 0, FL_SUBMENU},
		{"About Program", 0, about_cb},
		{0},
	{0}
};

int main(int argc, char* argv[])
{
	// Initialize rulestrings
	rs_init();

	// Load functions to lua
	lua_funcinit();

	// Load the configuration file first
	int errs = luaL_loadfile(lh, "config.lua");		// Load file
	lh.report_errors(errs);							// Report any errors in file
	errs = lua_pcall(lh, 0, LUA_MULTRET, 0);		// Run file
	lh.report_errors(errs);

	// Get all the numbers
	// Can only be called after you have ran the file
	get_config(lh);

	// Do the graphics after loading configuration file settings
	window = new Fl_Double_Window(gw*cw, gh*ch+menuh+buttonh, App_Title);
	window->begin();

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
	menu = new Fl_Menu_Bar(0, 0, window->w()+1, menuh);

	// The play button
	Fl_Button* play_bt = new Fl_Button(0, ch*gh+menuh, window->w()/2, buttonh, "@>");
	// The step button
	Fl_Button* step_bt = new Fl_Button(window->w()/2, ch*gh+menuh, window->w()/2, buttonh, "@->|");
	// The textbox
	Fl_Text_Editor* edit = new Fl_Text_Editor(window->w(), 0, txteditw, window->h());

	window->end();

	scriptbuf = new Fl_Text_Buffer;

	menu->copy(Menu_Items);
	play_bt->callback(play_cb);
	step_bt->callback(step_cb);
	edit->buffer(scriptbuf);
	edit->textfont(FL_COURIER);

	play_bt->tooltip("Play/Start the simulation");
	step_bt->tooltip("Step through simulation by one");

	// Position window in the middle
	window->position(Fl::w()/2-window->w()/2, Fl::h()/2-window->h()/2);

	window->callback(quit_cb);

	// Now load the autorun/functions file
	errs = luaL_loadfile(lh, "autorun.lua");	// Load file
	lh.report_errors(errs);							// Report any errors in file
	errs = lua_pcall(lh, 0, LUA_MULTRET, 0);		// Run file
	lh.report_errors(errs);

	window->show(argc, argv);

	return Fl::run();
}

void msgbox(const char* msg, const char* title=App_Title)
{
	fl_message_title(title);
	fl_message(msg);
}

void tick(void* step)
{
	int tempfx, tempfy, tempsx, tempsy;
	tempfx = 0; tempfy = 0; tempsx = gw-1; tempsy = gh-1;
	bool tempcells[gw*gh];				// Temporary array
	if(selectmode && tempdata[TD_CI] == -1)
	{
		tempfx = tempdata[TD_FX];
		tempfy = tempdata[TD_FY];
		tempsx = tempdata[TD_SX];
		tempsy = tempdata[TD_SY];
		for(int i=0; i<gw*gh; i++)
			tempcells[i] = cells[i]->getState();	// Initialize temperary array
	}
	for(int y=tempfy; y<=tempsy; y++)
	{
		for(int x=tempfx; x<=tempsx; x++)
		{
			int aroundtiles = 0;
			// The offsets
			for(short ofy=-1; ofy<2; ofy++)
			{
				for(short ofx=-1; ofx<2; ofx++)
				{
					if(((x+ofx>=0) && (y+ofy>=0) && (x+ofx <= tempsx) && (y+ofy <= tempsy) && !((ofx == 0) && (ofy == 0))))
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
			if(!projectmode)
				cells[i]->update_display();
		}
	}

	if(!(bool)step)
		Fl::repeat_timeout(timeout, tick);
}

void get_config(Lua_Helper L)
{
	// Check to see if the sizes are > 0
	// Only occurs if someone were to spoof
	// or if someone deleted a variable
	// If any of that happens, just use the default
	int temp = 0;
	temp = L.get<int>("txteditw");
	if(temp > 0 && temp)
	{
		txteditw = temp;
	}

	temp = L.get<int>("gw");
	if(temp > 0 && temp)
	{
		gw = temp;
	}

	temp = L.get<int>("gh");
	if(temp > 0 && temp)
	{
		gh = temp;
	}

	temp = L.get<int>("cw");
	if(temp > 0 && temp)
	{
		cw = temp;
	}

	temp = L.get<int>("ch");
	if(temp > 0 && temp)
	{
		ch = temp;
	}

	temp = L.get<int>("menuh");
	if(temp > 0 && temp)
	{
		menuh = temp;
	}

	temp = L.get<int>("buttonh");
	if(temp > 0 && temp)
	{
		buttonh = temp;
	}

	temp = L.get<int>("shadefactor");
	if(temp > 0 && temp)
	{
		shadefactor = temp;
	}

	temp = L.get<int>("backcol");
	if(temp > 0 && temp)
	{
		Tile::DeadCol = temp;
	}

	temp = L.get<int>("forecol");
	if(temp > 0 && temp)
	{
		Tile::AliveCol = temp;
	}

	double dtemp = L.get<double>("timeout");
	if(dtemp > 0 && dtemp)
	{
		timeout = dtemp;
	}
	// Successful no matter what because boolean value
	tutmode = L.get<bool>("tutmode");
}

inline int abs(int num)
{
	// The absolute value function
	// Returns negated number if smaller than zero
	if(num < 0)
		return -num;
	else
		return num;
}

void rs_init()
{
	// Puts all the rulestrings into a vector of vectors
	All_RS.push_back(GameofLife_RS);
	All_RS.push_back(HighLife_RS);
	All_RS.push_back(Maze_RS);
	All_RS.push_back(Mazectric_RS);
	All_RS.push_back(Replicator_RS);
}

void lua_funcinit()
{
	// Put functions into lua
	lua_pushcfunction(lh, lua_createrule);
	lua_setglobal(lh, "createrule");
	lua_pushcfunction(lh, lua_rgb_col);
	lua_setglobal(lh, "rgb_col");
	lua_pushcfunction(lh, lua_msgbox);
	lua_setglobal(lh, "msgbox");
	lua_pushcfunction(lh, lua_toggle);
	lua_setglobal(lh, "toggle");
}

int lua_createrule(lua_State* L)
{
	// (Hopefully) called by lua
	char* name = (char*)lua_tostring(L, 1);			// The name of rulestring (first argument)
	char* rs = (char*)lua_tostring(L, 2);			// The rulestring (second argument)
	lua_pop(L, 2);

	vector<int> My_RS;
	for(int i=0; rs[i] != '\0'; i++)
	{
		if(rs[i] == '/')
			My_RS.push_back(-1);
		else if(rs[i] >= '0' && rs[i] <= '8')
			My_RS.push_back(static_cast<int>(rs[i]-'0'));
		else
			cerr << "Error: Invalid number in rulestring." << endl;
	}

	string sname = "Rules/" + string(name);
	menu->add(sname.c_str(), 0, change_rule_cb, (void*)All_RS.size(), FL_MENU_RADIO);
	All_RS.push_back(My_RS);

	return 0;
}

int lua_rgb_col(lua_State* L)
{
	// Takes 3 arguments: r=[0,255], g=[0,255], b=[0,255]
	int r = (int)lua_tonumber(L, 1);
	int g = (int)lua_tonumber(L, 2);
	int b = (int)lua_tonumber(L, 3);
	int res = fl_rgb_color(r, g, b);
	// Pop out 3 arguments
	lua_pop(L, 3);

	lua_pushnumber(L, res);
	return 1;
}

int lua_msgbox(lua_State* L)
{
	// Takes 1 argument, 1 optional
	char* msg = (char*)lua_tostring(L, 1);		// The message
	if(lua_gettop(L) == 2)
	{
		msgbox(msg, lua_tostring(L, 2));
		lua_pop(L, 1);
	}
	else
	{
		msgbox(msg);
	}
	lua_pop(L, 1);
	return 0;
}

int lua_toggle(lua_State* L)
{
	// Takes 2 arguments, 1 optional
	if(lua_gettop(L) == 2)
	{
		// If there are only 2 arguments, just toggle
		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);
		cells[y*gw+x]->setState(!cells[y*gw+x]->getState());
		cells[y*gw+x]->update_display();
		lua_pop(L, 2);
	}
	else if(lua_gettop(L) == 3)
	{
		// If there is an extra argument (bool value), toggle that to bool value
		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);
		bool s = lua_toboolean(L, 3);
		cells[y*gw+x]->setState(s);
		cells[y*gw+x]->update_display();
		lua_pop(L, 3);
	}
	return 0;
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
	char* fn = fl_file_chooser("Save Board", "Cellular Automata (*.ca)| All Files (*.*)", ".");
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
	char* fn = fl_file_chooser("Open Board", "Cellular Automata (*.ca)| All Files (*.*)", ".");
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
	lh.close();
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
	if((long)data == 0)
	{
		if(All_RS[0].size() == 0)
		{
			// If you haven't established your own ruleset, just ask
			createrule_cb(w, data);
		}
	}
	else
	{
		// If ruleset exists, do it
		// If it doesn't exist, do Game of Life (1)
		int rsnum = abs((long)data);
		if(rsnum < All_RS.size())
			currentRS = All_RS[(long)data];
		else
			currentRS = All_RS[1];
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

		// Tutorial mode
		if(tutmode)
			msgbox("Cancelled");
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
	// Parameter format: "RS NAME"
	// Ask for rulestring
	// If rulestring is already a parameter, parse it
	char* dt = (char*)data;
	if(dt == 0)
	{
		dt = (char*)fl_input("Please enter your rulestring (Survival/Reproduction)\nExample\nGame of Life 23/3");
		if(dt == NULL)
			return;
	}

	// Parse the name
	bool spacegone = false;
	string sname = "";
	string srs = "";
	for(int i=0; dt[i] != 0; i++)
	{
		if(spacegone)
			sname += dt[i];
		else
			srs += dt[i];
		if(dt[i] == ' ')
			spacegone = true;
	}
	// If there is no name, ask for one
	if(sname == "")
	{
		char* name = (char*)fl_input("Please enter the name for the rulestring");
		if(name == NULL)
			return;
		sname = (string)name;
	}

	// Parse it down
	vector<int> My_RS;
	for(int i=0; srs.c_str()[i] != '\0'; i++)
	{
		if(srs.c_str()[i] == '/')
			My_RS.push_back(-1);
		else if(srs.c_str()[i] >= '0' && srs.c_str()[i] <= '8')
			My_RS.push_back(static_cast<int>(srs.c_str()[i]-'0'));
		else
			msgbox("Error: Invalid number in rulestring.");
	}

	sname = "Rules/"+sname;

	// Put everything into the menu
	menu->add(sname.c_str(), 0, change_rule_cb, (void*)All_RS.size(), FL_MENU_RADIO);

	// Put everything into vector
	All_RS.push_back(My_RS);

	// Tell user
	if(tutmode)
		msgbox("Rulestring has been successfully parsed into program. Please select it in menu");
}

void about_cb(Fl_Widget* w, void* data)
{
	msgbox(string(string(App_Title) + string("\nVersion: ") + string(App_Version) + string("\n\n") + string(App_Info)).c_str());
}

void project_cb(Fl_Widget* w, void* data)
{
	// Asks for how much you want to project into the future
	char* ask = (char*)fl_input("Projection");
	if(ask == NULL)
		return;

	// Converts string to long
	long times = strtol(ask, NULL, 10);
	if(times == 0)
		return;

	// Project mode
	projectmode = true;

	for(long l=0; l<times; l++)
	{
		tick((void*)true);
	}

	projectmode = false;

	// Update display
	for(int i=0; i<cells.size(); i++)
	{
		cells[i]->update_display();
	}
}

void toggle_console_cb(Fl_Widget* w, void* data)
{
	if(extendedmode)
	{
		// If the window is extended, put it back
		window->size(window->w()-txteditw, window->h());
		extendedmode = false;
	}
	else
	{
		// If the window is not extended, extend it
		window->size(window->w()+txteditw, window->h());
		extendedmode = true;
	}
}

void run_lua_script_cb(Fl_Widget* w, void* data)
{
	// Get the buffer text
	char* script = scriptbuf->text();

	// Load into lua
	int error = luaL_loadbuffer(lh, script, string(script).size(), "buffer") || lua_pcall(lh, 0, 0, 0);
	if(error)
	{
		// Print any errors
		msgbox(lua_tostring(lh, -1), "Lua Script");
		lua_pop(lh, 1);
	}
}

void os_script_cb(Fl_Widget* w, void* data)
{
	char const* txt = NULL;
	// Check for open/save file operation (slightly different, so put them together)
	if((long)data == 1)
	{
		// If save script
		txt = "Save Lua Script";
	}
	else
	{
		// If load script
		txt = "Open Lua Script";
	}
	// Get filename
	char* fn = fl_file_chooser(txt, "Lua File (*.lua)|All Files (*.*)", ".");
	if(fn == NULL)
		return;

	if((long)data == 1)
	{
		// Save file
		scriptbuf->savefile(fn);
	}
	else
	{
		// Load file
		scriptbuf->loadfile(fn);
	}
}
