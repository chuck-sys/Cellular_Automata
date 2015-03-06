/*
 * Tile.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: cheukyin699
 */

#include "Tile.h"

#include <FL/fl_draw.H>

// Initialize the static values
int Tile::AliveCol = FL_GREEN;
int Tile::DeadCol = FL_BLACK;
bool Tile::DragMode = false;

Tile::Tile(int x, int y, int w, int h, char* l):
    Fl_Button(x, y, w, h, l)
{
    this->state = false;
    this->update_display();
}

int Tile::handle(int event)
{
    switch(event)
    {
    case FL_RELEASE:
        this->do_callback();
        return 1;
    case FL_ENTER:
        if(Tile::DragMode)
        {
            // If the program is in drag mode
            this->do_callback();
            return 1;
        }
        else
            return 0;
    default:
        return 0;
    }
}

void Tile::draw()
{
    fl_draw_box(FL_FLAT_BOX, this->x(), this->y(), this->w(), this->h(), this->color());
}

void Tile::update_display()
{
    this->update_color();
    this->redraw();
}

void Tile::update_color()
{
    if(this->state)
    {
        // If the tile is switched on, the cell is alive
        // Change the color of the cell
        this->color(Tile::AliveCol);
    }
    else
    {
        // If the tile is not switched on, the cell is dead
        // Change the color of the cell
        this->color(Tile::DeadCol);
    }
}

