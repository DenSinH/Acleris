#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "VMath/Matrix.h"

#include <cmath>


#undef main
int main() {
    Acleris rasterizer(400, 400);

    float t = 0.0;

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        t += 2 * 3.1415 / 60;

        auto v0 = MakeVertex<4>({-0.5, 0.5, 1, 1}, MakeColor(0.0, 0.0, 1.0));
        auto v1 = MakeVertex<4>({0.5, 0.5, 1, 1}, MakeColor(1.0, 0.0, 0.0), 2);
        auto v2 = MakeVertex<4>({0, -0.5, 1, 2}, MakeColor(0.0, 1.0, 0.0), 2, 0);

        vmath::Matrix<float, 4, 4> mat{
                {std::cos(t), std::sin(t), 0, 0},
                {-std::sin(t), std::cos(t), 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1},
        };

        for (int i = 0; i < 200; i++) {
            (mat * Triangle(v0, v1, v2)).Fragment([](const Color& c) {
                return c;
            }).Draw(rasterizer);
        }

        Line(v0, v1).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);

        (mat * Point(v2)).Color(0xffff'ffff).Draw(rasterizer);
    });

    return 0;
}