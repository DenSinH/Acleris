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

    void DrawPointScreen(util::Point2D<int> p0);
    void DrawLineScreen(util::Point2D<int> p0, util::Point2D<int> p1);
    void DrawTriangleScreen(util::Point2D<int> p0, util::Point2D<int> p1, util::Point2D<int> p2);

private:
    template<bool xmajor>
    void DrawLineScreen(util::Point2D<int> p0, util::Point2D<int> p1);

    bool InBounds(int x, int y) const;
    bool InBounds(util::Point2D<int> p) const { return InBounds(p.x, p.y); };

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};