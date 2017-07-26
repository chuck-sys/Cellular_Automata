#ifndef TEMPVARS_H
#define TEMPVARS_H

#include "Utils.h"

struct Tempdata {
    Vector2D firstCorner;
    Vector2D secondCorner;
    int cornerIndex;
    Rect stamp;

    void reset() {
        firstCorner.reset();
        secondCorner.reset();
        cornerIndex = -1;
        stamp.reset();
    }
};

#endif
