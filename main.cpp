#include "acleris/Acleris.h"

#undef main
int main() {
    Acleris rasterizer(1280, 720);

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        rasterizer.DrawTriangleScreen({20, 20}, {400, 400}, {x, y});
    });
    return 0;
}
