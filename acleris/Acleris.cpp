#include "Acleris.h"


Acleris::Acleris(int width, int height) :
        width(width), height(height), screen{width, height}, zbuffer{width, height},
        view{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {
    Clear();
}

void Acleris::LookAt(const v3& eye, const v3& center, const v3& up) {
    const v3 f = (center - eye).normalize();
    const v3 u = up.normalize();
    const auto _f = f.data();
    const auto _u = u.data();
    const v3 s = v3{
        _f[1] * _u[2] - _f[2] * _u[1],
        _f[2] * _u[0] - _f[0] * _u[2],
        _f[0] * _u[1] - _f[1] * _u[0],
    }.normalize();

    v4 last_col = v4{
        s.dot(eye), u.dot(eye), f.dot(eye), 1
    };

    view = {s.extend<4>(), u.extend<4>(), f.extend<4>(), last_col};
}

void Acleris::Projection(float l, float r, float b, float t, float n, float f) {
    // see http://www.songho.ca/opengl/gl_projectionmatrix.html
    projection = {
            {(2 * n) / (r - l), 0,                 (r + l) / (r - l),   0},
            {0,                 (2 * n) / (t - b), (t + b) / (t - b),   0},
            {0,                 0,                 (-(f + n)) / (f - n), (-2 * f * n) / (f - n)},
            {0,                 0,                 -1,                  0},
    };
}

void Acleris::Projection(float r, float t, float n, float f) {
    projection = {
            {n / r, 0,     0,                    0},
            {0,     n / t, 0,                    0},
            {0,     0,     (-(f + n)) / (f - n), (-2 * f * n) / (f - n)},
            {0,     0,     -1,                   0},
    };
}

void* Acleris::SDLMakeWindow() const {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        throw util::FormatExcept("Error: %s", SDL_GetError());
    }

    auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | // SDL_WINDOW_RESIZABLE |
                                           SDL_WINDOW_ALLOW_HIGHDPI);
    auto window = SDL_CreateWindow("Acleris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
    return window;
}

void* Acleris::SDLMakeRenderer(void* window)  {
    return SDL_CreateRenderer(
            (SDL_Window*)window, -1, SDL_RENDERER_ACCELERATED
    );
}

void* Acleris::SDLMakeTexture(void* renderer) const {
    return SDL_CreateTexture(
            (SDL_Renderer*)renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, width, height
    );
}

void Acleris::SDLInit() {
    frontend.SDL.window = SDLMakeWindow();
    frontend.SDL.renderer = SDLMakeRenderer((SDL_Window*)frontend.SDL.window);
    frontend.SDL.texture = SDLMakeTexture((SDL_Renderer*)frontend.SDL.renderer);

    SDL_GL_SetSwapInterval(0);
}

void Acleris::SDLPollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
                SDL_DestroyWindow((SDL_Window*)frontend.SDL.window);
                SDL_Quit();
                frontend.shutdown = true;
                return;
            }
        }
    }
}

void Acleris::SDLPresent() {
    SDL_RenderClear((SDL_Renderer*)frontend.SDL.renderer);
    SDL_UpdateTexture((SDL_Texture*)frontend.SDL.texture, nullptr, (const void *)screen.data(), 4 * width);
    SDL_RenderCopy((SDL_Renderer*)frontend.SDL.renderer, (SDL_Texture*)frontend.SDL.texture, nullptr, nullptr);
    SDL_RenderPresent((SDL_Renderer*)frontend.SDL.renderer);
}

void Acleris::SDLUpdateMouseState() {
    SDL_GetMouseState(&mouse.x, &mouse.y);
}