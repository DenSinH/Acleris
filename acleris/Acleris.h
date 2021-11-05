#pragma once

#include <NArray.h>
#include <Point.h>

#include <vector>
#include <memory>
#include <functional>


struct Acleris {
public:
    using coord_t = float;

    const int width, height;
    util::NVect<uint32_t, 2> screen;

public:
    Acleris(int width, int height);

    void Clear();

    void SDLRun(std::function<void(int, int)> update);

    void DrawPointScreen(util::Point<int, 2> p0);
    void DrawLineScreen(util::Point<int, 2> p0, util::Point<int, 2> p1);
    void DrawTriangleScreen(util::Point<int, 2> p0, util::Point<int, 2> p1, util::Point<int, 2> p2);

private:
    template<bool xmajor>
    void DrawLineScreen(util::Point<int, 2> p0, util::Point<int, 2> p1);

    bool InBounds(int x, int y) const;
    bool InBounds(util::Point<int, 2> p) const { return InBounds(p.x[0], p.x[1]); };

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};