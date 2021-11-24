#include "Acleris.h"


Acleris::Acleris(int width, int height) :
        width(width), height(height), screen{width, height}, zbuffer{width, height} {
    Clear();
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