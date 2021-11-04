#include "Acleris.h"
#include "Format.h"
#include <SDL.h>

using namespace util;


Acleris::Acleris(int width, int height) :
        width(width), height(height), screen{width, height} {
    std::fill(screen.begin(), screen.end(), 0);
}

bool Acleris::InBounds(int x, int y) const {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
}

template<bool xmajor>
void Acleris::DrawLineScreen(Point2D<int> p0, Point2D<int> p1) {
    // for y-major lines, the algorithm is precisely the same, except x and y are swapped

    if constexpr(!xmajor) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
    }

    // from left to right
    if (p1.x < p0.x) {
        std::swap(p0, p1);
    }

    // initial coordinates
    double x = p0.x, y = p0.y;
    double dy = float(p1.y - p0.y) / float(p1.x - p0.x);

    if (x < 0) {
        // clip to screen boundary
        y += -x * dy;
        x = 0;
        if (p1.x < 0) [[unlikely]] return;  // entire line is off screen
    }
    else if (x > width) {
        return;
    }

    if (y < 0) {
        if (dy < 0) [[unlikely]] return;  // entire line is off screen
        x += -y / dy;
        if (x > p1.x) [[unlikely]] return;  // line ended before reaching screen
        y = 0;
    }
    else if (y > height) {
        if (dy > 0) [[unlikely]] return;  // entire line is off screen
        x += (y - height) / dy;
        if (x > p1.x) [[unlikely]] return;  // line ended before reaching screen
        y = height;
    }

    // need to flip coordinates for x/y major
    if constexpr(xmajor) {
        for (int x_ = x; x_ < p1.x && InBounds(x_, y); x_++) {
            screen(int(x_), int(y)) = 0xffff'ffff;
            y += dy;
        }
    }
    else {
        for (int x_ = x; x_ < p1.x && InBounds(y, x_); x_++) {
            screen(int(y), int(x_)) = 0xffff'ffff;
            y += dy;
        }
    }
}

void Acleris::DrawLineScreen(Point2D<int> p0, Point2D<int> p1) {
    // find out if the line is x-major or y-major
    // |y1 - y0| < |x1 - x0| <==> x-major (not steep)
    bool xmajor = std::abs(p1.x - p0.x) >= std::abs(p1.y - p0.y);
    if (xmajor) {
        DrawLineScreen<true>(p0, p1);
    }
    else {
        DrawLineScreen<false>(p0, p1);
    }
}

void* Acleris::SDLMakeWindow() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        throw util::FormatExcept("Error: %s", SDL_GetError());
    }

    auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | // SDL_WINDOW_RESIZABLE |
                                           SDL_WINDOW_ALLOW_HIGHDPI);
    auto window = SDL_CreateWindow("Acleris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
    return window;
}

void* Acleris::SDLMakeRenderer(void* window) {
    return SDL_CreateRenderer(
            (SDL_Window*)window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
}

void* Acleris::SDLMakeTexture(void* renderer) const {
    return SDL_CreateTexture(
            (SDL_Renderer*)renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, width, height
    );
}

void Acleris::SDLRun() {
    auto window = (SDL_Window*)SDLMakeWindow();
    auto renderer = (SDL_Renderer*)SDLMakeRenderer(window);
    auto texture = (SDL_Texture*)SDLMakeTexture(renderer);

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return;
                }
            }

            SDL_RenderClear(renderer);
            SDL_UpdateTexture(texture, nullptr, (const void *)screen.data(), 4 * width);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }
    }
}