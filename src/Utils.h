#ifndef UTILS_H
#define UTILS_H

struct Vector2D {
    int x;
    int y;

    void reset() {
        x = -1;
        y = -1;
    }
};

struct Rect {
    Vector2D pos;
    Vector2D size;

    void reset() {
        pos.reset();
        size.reset();
    }
};

#endif
