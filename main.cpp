#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "acleris/Matrix.h"


#undef main
int main() {
    Acleris rasterizer(1280, 720);

    Matrix<float, 2, 2> mat{
             {0.5, 0},
             {0, 0.5}
    };

    Matrix<float, 2, 2> mat2{
             {0, 2},
             {2, 0}
    };

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        auto v0 = MakeVertex<float, 2>({0.25, 0.25}, Color{0.0, 1.0, 0.0}, 2, 0);
        auto v1 = MakeVertex<float, 2>({0.75, 0.75}, Color{1.0, 0.0, 0.0}, 2);
        auto v2 = MakeVertex<float, 2>({1, 0}, Color{0.0, 0.0, 1.0});

        (mat * Triangle(v0, v1, v2)).Fragment([](Color c) {
            return c;
        }).Draw(rasterizer);

        (mat2 * Line(v0, v1)).Fragment([](Color c) {
            return c;
        }).Draw(rasterizer);

    });

    return 0;
}