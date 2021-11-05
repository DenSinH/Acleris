#include "acleris/Acleris.h"
#include "acleris/common/Vertex.h"

#undef main
int main() {
    Acleris rasterizer(1280, 720);

//    rasterizer.SDLRun([&](int x, int y) {
//        rasterizer.Clear();
//        rasterizer.DrawTriangleScreen({20, 20}, {400, 400}, {x, y});
//    });

    auto v0 = MakeVertex<float, 2>({0, 0}, 1.0, 2.0);
    auto v1 = MakeVertex<float, 2>({0, 1}, 1.0, 2.0);
    auto v2 = MakeVertex<float, 2>({1, 0}, 1.0, 2.0);

    auto color = Triangle(v0, v1, v2).Fragment(
        0.5, 0.5, [] (float x, float y) -> int {
            return (int) 10 * (x + y);
        }
    );

    auto bc = BarycentricInterp<float>(0.5, 0.5, v0, v1, v2);
    std::printf("%f %f\n", bc.first, bc.second);
    std::printf("%d\n", color);

    return 0;
}
