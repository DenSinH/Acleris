#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "acleris/Matrix.h"

#include <cmath>


#undef main
int main() {
    Acleris rasterizer(1280, 720);

    float t = 0.0;

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        t += 0.01;

        auto v0 = MakeVertex<float, 2>({1, 0}, Color{0.0, 0.0, 1.0});
        auto v1 = MakeVertex<float, 2>({0.75, 0.75}, Color{1.0, 0.0, 0.0}, 2);
        auto v2 = MakeVertex<float, 2>({0.25, 0.25}, Color{0.0, 1.0, 0.0}, 2, 0);

        Matrix<float, 2, 2> mat{
                {std::cos(t), std::sin(t)},
                {-std::sin(t), std::cos(t)}
        };

        auto offs = Vector<float, 2>{0.5, 0.5};
        (mat * (Triangle(v0, v1, v2) - offs) + offs).Fragment([](Color c) {
            return c;
        }).Draw(rasterizer);

        ((Line(v0, v1) - offs) + 0.1 * mat * offs).Fragment([](Color c) {
            return c;
        }).Draw(rasterizer);

    });

    return 0;
}