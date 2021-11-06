#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "acleris/Shapes.h"

#undef main
int main() {
    Acleris rasterizer(1280, 720);

    rasterizer.SDLRun([&](int x, int y) {
        rasterizer.Clear();
        auto v0 = MakeVertex<float, 2>({0.25, 0.25}, 0.0, 1.0, 0.0, 2, 0);
        auto v1 = MakeVertex<float, 2>({0.75, 0.75}, 1.0, 0.0, 0.0, 2);
        auto v2 = MakeVertex<float, 2>({1, 0}, 0.0, 0.0, 1.0);

//        Point(v0).Color(0xffff'ffff).Draw(rasterizer);
//        Line(v0, v1).Fragment([](float x) {
//            uint32_t scale = x * 0xff;
//            return 0x0000'00ff | ((scale << 24) | (scale << 16) | (scale << 8));
//        }).Draw(rasterizer);

        Triangle(v0, v1, v2).Fragment([](float x, float y, float z) {
            uint32_t r = x * 0xff;
            uint32_t g = y * 0xff;
            uint32_t b = z * 0xff;
            return 0x0000'00ff | ((r << 24) | (g << 16) | (b << 8));
        }).Draw(rasterizer);

    });

//    auto v0 = MakeVertex<float, 2>({0, 0}, 1.0, 2.0);
//    auto v1 = MakeVertex<float, 2>({0, 1}, 1.0, 2.0);
//    auto v2 = MakeVertex<float, 2>({1, 0}, 1.0, 2.0);
//
//    auto color = Triangle(v0, v1, v2).Fragment(
//        0.5, 0.5, [] (float x, float y) -> int {
//            return (int) 10 * (x + y);
//        }
//    );
//
//    auto bc = BarycentricInterp<float>(0.5, 0.5, v0, v1, v2);
//    std::printf("%f %f\n", bc.first, bc.second);
//    std::printf("%d\n", color);

    return 0;
}
