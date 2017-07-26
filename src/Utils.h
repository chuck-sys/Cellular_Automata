#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <map>
#include <string>

using namespace std;

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

vector<map<string, vector<string>>> getStamps();
int abs(int);

#endif
