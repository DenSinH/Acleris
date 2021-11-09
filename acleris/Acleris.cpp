#include "Acleris.h"


Acleris::Acleris(int width, int height) :
        width(width), height(height), screen{width, height} {
    Clear();
}

bool Acleris::InBounds(int x, int y) const {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
}

void Acleris::Clear() {
    std::fill(screen.begin(), screen.end(), 0);
}
//
//template<typename V0>
//void Acleris::Point2D(V0 v0) {
//    static_assert(V0::dim == 2);
//
//    if (InBounds(v0)) {
//        screen(v0.x[0], v0.x[1]) = std::app;
//    }
//}

//template<bool xmajor>
//void Acleris::DrawLineScreen(Point<int, 2> p0, Point<int, 2> p1) {
//
//}
//
//void Acleris::DrawLineScreen(Point<int, 2> p0, Point<int, 2> p1) {
//    // find out if the line is x-major or y-major
//    // |y1 - y0| < |x1 - x0| <==> x-major (not steep)
//    bool xmajor = std::abs(p1.x[0] - p0.x[0]) >= std::abs(p1.x[1] - p0.x[1]);
//    if (xmajor) {
//        DrawLineScreen<true>(p0, p1);
//    }
//    else {
//        DrawLineScreen<false>(p0, p1);
//    }
//}
//
//void Acleris::DrawTriangleScreen(Point<int, 2> p0, Point<int, 2> p1, Point<int, 2> p2) {
//    // sort by y coordinate
//    if (p1.x[1] < p0.x[1]) {
//        std::swap(p0, p1);
//    }
//    if (p2.x[1] < p0.x[1]) {
//        std::swap(p0, p2);
//    }
//    if (p2.x[1] < p1.x[1]) {
//        std::swap(p1, p2);
//    }
//    // first: go from top point to middle point
//    coord_t x1 = p0.x[0], x2 = p0.x[0];  // x1 will end up at p1 and x2 at p2
//    coord_t dx2 = coord_t(p2.x[0] - p0.x[0]) / coord_t(p2.x[1] - p0.x[1]);
//    coord_t dx1 = coord_t(p1.x[0] - p0.x[0]) / coord_t(p1.x[1] - p0.x[1]);
//
//    coord_t y = p0.x[1];
//    if (y < 0) {
//        x1 += -y * dx1;
//        x2 += -y * dx2;
//        y = 0;
//    }
//
//    // draw part from p0 down to p1 (along p0 -- p1 and p0 -- p2)
//    const int ymax = std::min(height, p1.x[1]);
//    while (y < ymax) {
//        const int xmax = std::min<int>(width, std::max(x1, x2) + 1);
//        for (int x = std::max<int>(0, std::min(x1, x2)); x < xmax; x++) {
//            screen(x, y) = 0xffff'ffff;
//        }
//        x1 += dx1;
//        x2 += dx2;
//        y++;
//    }
//
//    // draw part from p1 down to p2 (along p0 -- p2 and p1 -- p2)
//    const int ymax_ = std::min(height, std::max(p1.x[1], p2.x[1]));
//    x1 = p1.x[0];  // just to be sure (should already be approx. the right value)
//    dx1 = coord_t(p2.x[0] - p1.x[0]) / coord_t(p2.x[1] - p1.x[1]);
//
//    while (y < ymax_) {
//        const int xmax = std::min<int>(width, std::max(x1, x2) + 1);
//        for (int x = std::max<int>(0, std::min(x1, x2)); x < xmax; x++) {
//            screen(x, y) = 0xffff'ffff;
//        }
//        x1 += dx1;
//        x2 += dx2;
//        y++;
//    }
//}

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
            (SDL_Window*)window, -1, SDL_RENDERER_ACCELERATED
    );
}

void* Acleris::SDLMakeTexture(void* renderer) const {
    return SDL_CreateTexture(
            (SDL_Renderer*)renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, width, height
    );
}

void Acleris::SDLRun(std::function<void(int, int)> update) {
    auto window = (SDL_Window*)SDLMakeWindow();
    auto renderer = (SDL_Renderer*)SDLMakeRenderer(window);
    auto texture = (SDL_Texture*)SDLMakeTexture(renderer);

    SDL_GL_SetSwapInterval(0);

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
        }

        int x, y;
        SDL_GetMouseState(&x, &y);

        update(x, y);

        SDL_RenderClear(renderer);
        SDL_UpdateTexture(texture, nullptr, (const void *)screen.data(), 4 * width);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}