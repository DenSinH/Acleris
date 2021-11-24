#pragma once

#include "util/Format.h"
#include "util/NArray.h"
#include "Vertex.h"
#include "VMath/Matrix.h"

#include <SDL.h>

#include <vector>
#include <memory>
#include <functional>
#include <atomic>


struct Clear {
    enum : std::uint32_t {
        Screen = 1,
        Depth = 2,
    };
};

struct Acleris {
public:
    using coord_t = float;

    const std::uint32_t width, height;
    util::NVect<std::uint32_t, 2> screen;
    util::NVect<std::atomic<float>, 2> zbuffer;

public:
    Acleris(int width, int height);

    template<std::uint32_t flags = Clear::Screen | Clear::Depth>
    void Clear() {
        if constexpr((flags & Clear::Screen) != 0) {
            std::fill(screen.begin(), screen.end(), 0);
        }
        if constexpr((flags & Clear::Depth) != 0) {
            std::fill(zbuffer.begin(), zbuffer.end(), 0);
        }
    }

    void SDLRun(std::function<void(int, int)> update);

    bool InBounds(const vmath::Vector<std::uint32_t, 2>& v0) const {
        using uiv32 = vmath::Vector<std::uint32_t, 2>;
        return (((v0 < uiv32{}) | (v0 >= uiv32{width, height})) & (uiv32{-1, -1})).all_zero();
    };

    bool InBounds(const vmath::Vector<std::uint32_t, 3>& v0) const {
        return InBounds(vmath::Matrix<std::uint32_t, 2, 3>{{1, 0, 0}, {0, 1, 0}} * v0);
    };

    bool InBounds(const vmath::Vector<std::uint32_t, 4>& v0) const {
        return InBounds(vmath::Matrix<std::uint32_t, 2, 4>{{1, 0, 0, 0}, {0, 1, 0, 0}} * v0);
    };

    bool InBounds(int x, int y) const {
        return (x >= 0) && (x < width) && (y >= 0) && (y < height);
    }

    template<class Comp>
    bool CmpExchangeZ(float z, int x, int y) {
        Comp comp;
        float current_z;
        auto& dest = zbuffer(x, y);
        do {
            current_z = dest.load();
            if (!comp(z, current_z)) {
                return false;
            }
        } while(!dest.compare_exchange_weak(current_z, z));
        return true;
    }

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};
