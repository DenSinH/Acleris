#include "acleris/Acleris.h"
#include "acleris/Shapes.h"
#include "acleris/DrawList.h"
#include "VMath/Matrix.h"

#include <cmath>
#include <algorithm>


#undef main
int main() {
    Acleris rasterizer(800, 800);
    rasterizer.Projection(0.1, 0.1, 0.1, 100.0);

    DrawList list(rasterizer);

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
        t += dt;

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

        list << (mat * Triangle(vert0, vert1, vert2)).Fragment([](const Color& c) {
            return c;
        });
        list << (mat * Triangle(vert0, vert1, vert3)).Fragment([](const Color& c) {
            return c;
        });
        list << (mat * Triangle(vert0, vert2, vert3)).Fragment([](const Color& c) {
            return c;
        });
        list << (mat * Triangle(vert1, vert2, vert3)).Fragment([](const Color& c) {
            return c;
        });

        list << Line(MakeVertex<3>(v3{-1, 0, 0}), MakeVertex<3>(v3{1, 0, 0})).Color(RGB(1, 0, 0));
        list << Line(MakeVertex<3>(v3{0, -1, 0}), MakeVertex<3>(v3{0, 1, 0})).Color(RGB(0, 1, 0));
        list << Line(MakeVertex<3>(v3{0, 0, -1}), MakeVertex<3>(v3{0, 0, 1})).Color(RGB(0, 0, 1));

        list << Point(MakeVertex<3>(v3{2, 2, 0})).Color(0xffff'ffff);

        list.Wait();
    });

    return 0;
}