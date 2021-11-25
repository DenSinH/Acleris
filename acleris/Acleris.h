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
    vmath::Matrix<float, 4, 4> view;
    vmath::Matrix<float, 4, 4> projection;

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

    template<class Comp = std::less<float>>
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

    void LookAt(const v3& eye, const v3& center, const v3& up);
    void Projection(float left, float right, float bottom, float top, float near, float far);
    void Projection(float right, float top, float near, float far);

    template<size_t n, typename... Args>
    v4 DeviceCoordinates(const Vertex<n, Args...>& vert) {
        // extend vertex position with w = 1 (if length is not yet 4)
        v4 result = vert.x.template extend<4>();
        if constexpr(n < 4) {
            // set w = 1
            result = (result.reinterpret<std::uint32_t>() | vmath::Vector<float, 4>{0, 0, 0, 1}.reinterpret<std::uint32_t>()).reinterpret<float>();
        }

        // multiply with model, view and projection matrices
        result = projection * (view * /* model * */ result);

        // project onto screen
        const v4 screen_dim = {width, height, 1, 1};
        result = (v4{1, 1, 0, 0} + util::Project(result)) * screen_dim * v4{0.5, 0.5, 1, 1};

        return result;
    }

    void* SDLMakeWindow();
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
};
