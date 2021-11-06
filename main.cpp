#include "acleris/Acleris.h"
#include "acleris/Shapes.h"


#undef main
int main() {
    Acleris rasterizer(1280, 720);

    float offs = 0.0;
    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        offs += 0.1;
        auto v0 = MakeVertex<float, 2>({0.25, 0.25f + offs}, Color{0.0, 1.0, 0.0}, 2, 0);
        auto v1 = MakeVertex<float, 2>({0.75, 0.75}, Color{1.0, 0.0, 0.0}, 2);
        auto v2 = MakeVertex<float, 2>({1, 0}, Color{0.0, 0.0, 1.0});

        Triangle(v0, v1, v2).Fragment([](Color c) {
            return c;
        }).Draw(rasterizer);

        Line(v0, v1).Fragment([](Color c) {
            return c;
        }).Draw(rasterizer);

    });

    return 0;
}
