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

    const int width, height;
    util::NVect<uint32_t, 2> screen;

public:
    Acleris(int width, int height);

    void Clear();

    void SDLRun(std::function<void(int, int)> update);

    bool InBounds(int x, int y) const;
    template<typename V0>
    bool InBounds(V0 v0) const { return InBounds(v0.x[0], v0.x[1]); };

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};
