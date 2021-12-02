#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "VMath/Matrix.h"

#include <cmath>
#include <algorithm>


#undef main
int main() {
    Acleris rasterizer(800, 800);
    rasterizer.Projection(0.1, 0.1, 0.1, 100.0);

    constexpr float mouse_move = 0.005;
    constexpr float pi = 3.14159265;
    constexpr float dt = 0.5 * 3.1415 / 60;
    constexpr float dx = 0.05;

    float t = 0.5;
    float theta = pi / 2;
    float phi = -pi/2;

    Acleris::Mouse prev = rasterizer.mouse;

    v3 pos = {0, 0, 1};
    rasterizer.SDLRun([&](Acleris::Mouse mouse, Acleris::Keyboard keyboard) {
        rasterizer.Clear();
//        t += dt;

        // update mouse state
        if (mouse.x != prev.x) {
            phi += mouse_move * (mouse.x - prev.x);
            phi = std::fmod(phi, 2 * pi);
        }
        if (mouse.y != prev.y) {
            theta += mouse_move * (mouse.y - prev.y);
            theta = std::clamp<float>(theta, 0.01, pi - 0.01);
        }
        prev = mouse;

        // update view matrix
        // spherical coordinates
        v3 look = {
                std::sin(theta) * std::cos(phi),
                std::cos(theta),
                std::sin(theta) * std::sin(phi)
        };
//        v3 up = {
//                std::sin(theta - pi / 2) * std::cos(phi),
//                std::cos(theta - pi / 2),
//                std::sin(theta - pi / 2) * std::sin(phi)
//        };

        // move
        if (keyboard.ascii['w']) {
            pos += dx * (m3x3{{1, 0, 0}, {0, 0, 0}, {0, 0, 1}} * look).normalize();
        }
        if (keyboard.ascii['s']) {
            pos -= dx * (m3x3{{1, 0, 0}, {0, 0, 0}, {0, 0, 1}} * look).normalize();
        }
        if (keyboard.ascii['d']) {
            // "cross product with up"
            pos -= dx * (m3x3{{0, 0, 1}, {0, 0, 0}, {-1, 0, 0}} * look);
        }
        if (keyboard.ascii['a']) {
            // "cross product with up"
            pos += dx * (m3x3{{0, 0, 1}, {0, 0, 0}, {-1, 0, 0}} * look);
        }
        if (keyboard.space) {
            pos += dx * v3{0, 1, 0};
        }
        if (keyboard.lshift) {
            pos -= dx * v3{0, 1, 0};
        }
        rasterizer.LookAt(-pos, -pos + look, v3{0, 1, 0});

        // draw geometry
        auto vert0 = MakeVertex<3>({-0.5, 0.5, 0}, RGB(1.0, 0.0, 0.0));
        auto vert1 = MakeVertex<3>({ 0.5, 0.5, 0}, RGB(0.0, 1.0, 0.0));
        auto vert2 = MakeVertex<3>({   0,-0.5, 0}, RGB(0.0, 0.0, 1.0));
        auto vert3 = MakeVertex<3>({   0, 0, 0.5}, RGB(1.0, 0.0, 1.0));

        m3x3 mat{
            {std::cos(t), std::sin(t), 0},
            {-std::sin(t), std::cos(t), 0},
            {0, 0, 1},
        };

//        printf("\n");
//        printf("%f %f %f %f\n", rasterizer.view.get<0, 0>(), rasterizer.view.get<0, 1>(), rasterizer.view.get<0, 2>(), rasterizer.view.get<0, 3>());
//        printf("%f %f %f %f\n", rasterizer.view.get<1, 0>(), rasterizer.view.get<1, 1>(), rasterizer.view.get<1, 2>(), rasterizer.view.get<1, 3>());
//        printf("%f %f %f %f\n", rasterizer.view.get<2, 0>(), rasterizer.view.get<2, 1>(), rasterizer.view.get<2, 2>(), rasterizer.view.get<2, 3>());
//        printf("%f %f %f %f\n", rasterizer.view.get<3, 0>(), rasterizer.view.get<3, 1>(), rasterizer.view.get<3, 2>(), rasterizer.view.get<3, 3>());

        (mat * Triangle(vert0, vert1, vert2)).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);
        (mat * Triangle(vert0, vert1, vert3)).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);
        (mat * Triangle(vert0, vert2, vert3)).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);
        (mat * Triangle(vert1, vert2, vert3)).Fragment([](const Color& c) {
            return c;
        }).Draw(rasterizer);

        Line(MakeVertex<3>(v3{-1, 0, 0}), MakeVertex<3>(v3{1, 0, 0})).Color(RGB(1, 0, 0)).Draw(rasterizer);
        Line(MakeVertex<3>(v3{0, -1, 0}), MakeVertex<3>(v3{0, 1, 0})).Color(RGB(0, 1, 0)).Draw(rasterizer);
        Line(MakeVertex<3>(v3{0, 0, -1}), MakeVertex<3>(v3{0, 0, 1})).Color(RGB(0, 0, 1)).Draw(rasterizer);
//
//        Point(MakeVertex<3>(pos + look)).Color(0xffff'ffff).Draw(rasterizer);
    });

    return 0;
}