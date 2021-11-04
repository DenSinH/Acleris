#pragma once

#include <NArray.h>
#include <Point.h>

#include <vector>
#include <memory>


struct Acleris {
public:
    const int width, height;
    util::NVect<uint32_t, 2> screen;

public:
    Acleris(int width, int height);

    void SDLRun();

    void DrawLineScreen(util::Point2D<int> p0, util::Point2D<int> p1);

private:
    template<bool xmajor>
    void DrawLineScreen(util::Point2D<int> p0, util::Point2D<int> p1);

    bool InBounds(int x, int y) const;

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};