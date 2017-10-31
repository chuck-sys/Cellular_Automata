#ifndef SRC_TILE_H_
#define SRC_TILE_H_

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

class Tile: public Fl_Button
{
private:
    bool state;
    int coords[2];

public:
    static int AliveCol;
    static int DeadCol;
    static bool DragMode;

    Tile(int x, int y, int w, int h, char* l=0);

    int handle(int);                // Overrides the default event handler
    void draw();                    // Overrides the default draw function
    void update_display();
    void update_color();

    void toggle() {this->state = !this->state;};
    void setState(bool s) {this->state = s;};
    bool getState() {return this->state;};
    int getX() {return this->coords[0];};
    int getY() {return this->coords[1];};
    int* getCoords() {return this->coords;};
    void setCoords(int x, int y) {coords[0] = x; coords[1] = y;};
};

#endif /* SRC_TILE_H_ */
