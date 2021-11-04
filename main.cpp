#include "acleris/Acleris.h"

#undef main
int main() {
    Acleris rasterizer(1280, 720);

    rasterizer.DrawLineScreen({0, 0}, {1280, 720});
    rasterizer.DrawLineScreen({-720, 0}, {720, 1280});
    rasterizer.DrawLineScreen({-720, -1280}, {720, 1280});
    rasterizer.DrawLineScreen({360, -1280}, {360, 1280});
    rasterizer.DrawLineScreen({0, 620}, {1280, 620});

    rasterizer.SDLRun();
    return 0;
}
