#pragma once

#include "util/NArray.h"
#include "util/Vector.h"
#include "Vertex.h"
#include "VMath/Matrix.h"

#include <SDL.h>

#include <vector>
#include <memory>
#include <functional>
#include <atomic>


namespace detail {

/*
 * For getting function argument types.
 * */

template<typename... Args>
struct pack {

};

template<typename Callable> struct func;

template<typename R, typename... Args>
struct func<R(Args...)> {
    using args_t = pack<Args...>;
};

template<typename R, typename... Args>
struct func<R (*)(Args...)> {
    using args_t = pack<Args...>;
};

template<typename R, typename C, typename... Args>
struct func<R (C::*)(Args...)> {
    using args_t = pack<Args...>;
};

template<typename R, typename C, typename... Args>
struct func<R (C::*)(Args...) const> {
    using args_t = pack<Args...>;
};

template<typename Callable>
struct func {
    using args_t = typename func<decltype(&Callable::operator())>::args_t;
};

}

struct Clear {
    enum : std::uint32_t {
        Screen = 1,
        Depth = 2,
    };
};

struct Clip {
    enum Direction {
        None = 0, Right, Left, Top, Bottom
    };
};

struct Acleris {
    const std::uint32_t width, height;
    util::NVect<std::uint32_t, 2> screen;
    util::NVect<std::atomic<float>, 2> zbuffer;
    m4x4 view;
    float near, far;
    m4x4 projection;

    Acleris(int width, int height);

    template<std::uint32_t flags = Clear::Screen | Clear::Depth>
    void Clear() {
        if constexpr((flags & Clear::Screen) != 0) {
            std::fill(screen.begin(), screen.end(), 0);
        }
        if constexpr((flags & Clear::Depth) != 0) {
            std::fill(zbuffer.begin(), zbuffer.end(), std::numeric_limits<float>::infinity());
        }
    }

    template<class F>
    void SDLRun(const F& update);

    template<size_t n>
    bool InBounds(const vmath::Vector<std::uint32_t, n>& v0) const {
        using uiv32 = vmath::Vector<std::uint32_t, 2>;

        uiv32 _v0 = v0.template shorten<2>();
        return (((_v0 < uiv32{}) | (_v0 >= uiv32{width, height})) & (uiv32{-1, -1})).all_zero();
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
    v4 DeviceCoordinates(const Vertex<n, Args...>& vert) const {
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

    template<typename T, size_t n>
    Clip::Direction Clip(const vmath::Vector<T, n>& v) const {
        const T x = v.template get<0>();
        const T y = v.template get<1>();
        if (x < 0) return Clip::Left;
        if (x > width) return Clip::Right;
        if (y < 0) return Clip::Top;
        if (y > height) return Clip::Bottom;
        return Clip::None;
    }

    struct Keyboard {
        bool ascii[128] = {};
        bool space, lshift;
        bool up, down, left, right;
        bool esc;
    } keyboard = {};

    struct Mouse {
        int x, y;
    } mouse = {};

private:
    struct {
        struct {
            void* window;
            void* renderer;
            void* texture;
            bool mouse_trapped = true;
        } SDL;

        bool shutdown = false;
    } frontend;

    template<typename T>
    T GetArg() {
        if constexpr(std::is_same_v<T, Acleris::Mouse>) {
            return mouse;
        }
        else if constexpr(std::is_same_v<T, Acleris::Keyboard>) {
            return keyboard;
        }
        else {
            []<bool b = false>{ static_assert(b, "Bad argument for run function"); }();
        }
    }

    template<typename... Args>
    std::tuple<Args...> GetArgs(detail::pack<Args...>) {
        return std::tuple<Args...>{GetArg<Args>()...};
    }

    template<typename T, typename... Args>
    static constexpr bool Need(detail::pack<Args...>) {
        return (std::is_same_v<T, Args> || ...);
    }

    void* SDLMakeWindow() const;
    static void* SDLMakeRenderer(void* window);
    void* SDLMakeTexture(void* renderer) const;
    void SDLInit();
    void SDLPollEvents();
    void SDLPresent();
};


template<class F>
void Acleris::SDLRun(const F& update) {
    SDLInit();

    while (!frontend.shutdown) {
        SDLPollEvents();

        auto pack = typename detail::func<F>::args_t{};
        std::apply(update, GetArgs(pack));

        SDLPresent();
    }
}