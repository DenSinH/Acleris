#pragma once

#include "util/Format.h"
#include "util/NArray.h"
#include "Vertex.h"

#include <SDL.h>

#include <vector>
#include <memory>
#include <functional>


struct Acleris {
public:
    using coord_t = float;

    const std::uint32_t width, height;
    util::NVect<std::uint32_t, 2> screen;

public:
    Acleris(int width, int height);

    void Clear();

    void SDLRun(std::function<void(int, int)> update);

    bool InBounds(const vmath::Vector<std::uint32_t, 2>& v0) const {
        using uiv32 = vmath::Vector<std::uint32_t, 2>;
        return (((v0 < uiv32{}) | (v0 >= uiv32{width, height})) & (uiv32{-1, -1})).all_zero();
    };

    bool InBounds(int x, int y) const {
        return (x >= 0) && (x < width) && (y >= 0) && (y < height);
    }

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};
