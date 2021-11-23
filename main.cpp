#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "VMath/Matrix.h"

#include <cmath>


#undef main
int main() {
    Acleris rasterizer(1280, 720);

    float t = 0.0;

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        t += 0.01;

        auto v0 = MakeVertex<2>({1, 0}, MakeColor(0.0, 0.0, 1.0));
        auto v1 = MakeVertex<2>({0.75, 0.75}, MakeColor(1.0, 0.0, 0.0), 2);
        auto v2 = MakeVertex<2>({0.25, 0.25}, MakeColor(0.0, 1.0, 0.0), 2, 0);

        vmath::Matrix<float, 2, 2> mat{
                {std::cos(t), std::sin(t)},
                {-std::sin(t), std::cos(t)}
        };

        auto offs = vmath::Vector<float, 2>{0.5, 0.5};
        for (int i = 0; i < 10; i++) {
            (mat * (Triangle(v0, v1, v2) - offs) + offs).Fragment([](const Color& c) {
                return c;
            }).Draw(rasterizer);
        }

        ((Line(v0, v1) - offs) + 0.1 * mat * offs).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);

        (mat * (Point(v2) - offs) + offs).Color(0xffff'ffff).Draw(rasterizer);
    });

    return 0;
}