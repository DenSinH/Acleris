#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "VMath/Matrix.h"

#include <cmath>


#undef main
int main() {
    Acleris rasterizer(800, 800);

    float t = 0.0;

    rasterizer.Projection(1.0, 1.0, 1.0, 100.0);

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        t += 2 * 3.1415 / 60;

        auto v0 = MakeVertex<3>({-0.5, 0.5, 0}, RGB(1.0, 0.0, 0.0));
        auto v1 = MakeVertex<3>({ 0.5, 0.5, 0}, RGB(0.0, 1.0, 0.0));
        auto v2 = MakeVertex<3>({   0,-0.5, 0}, RGB(0.0, 0.0, 1.0), 0);

        vmath::Matrix<float, 3, 3> mat{
                {std::cos(t), std::sin(t), 0},
                {-std::sin(t), std::cos(t), 0},
                {0, 0, 1},
        };

        rasterizer.LookAt({0, 0, std::cos(t)}, {0, 0, std::cos(t) - 1}, {0, 1, 0});
//        printf("\n");
//        printf("%f %f %f %f\n", rasterizer.view.get<0, 0>(), rasterizer.view.get<0, 1>(), rasterizer.view.get<0, 2>(), rasterizer.view.get<0, 3>());
//        printf("%f %f %f %f\n", rasterizer.view.get<1, 0>(), rasterizer.view.get<1, 1>(), rasterizer.view.get<1, 2>(), rasterizer.view.get<1, 3>());
//        printf("%f %f %f %f\n", rasterizer.view.get<2, 0>(), rasterizer.view.get<2, 1>(), rasterizer.view.get<2, 2>(), rasterizer.view.get<2, 3>());
//        printf("%f %f %f %f\n", rasterizer.view.get<3, 0>(), rasterizer.view.get<3, 1>(), rasterizer.view.get<3, 2>(), rasterizer.view.get<3, 3>());

        for (int i = 0; i < 20; i++) {
            (mat * Triangle(v0, v1, v2)).Fragment([](const Color& c) {
                return c;
            }).Draw(rasterizer);
        }

        Line(v0, v1).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);

        // todo: segfault
        (mat * Point(v2)).Color(0xffff'ffff).Draw(rasterizer);
    });

    return 0;
}