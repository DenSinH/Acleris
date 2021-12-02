#include "Acleris.h"

#include <stdexcept>


Acleris::Acleris(int width, int height) :
        width(width), height(height), screen{width, height}, zbuffer{width, height},
        regions(width / RegionSize, height / RegionSize),
        view{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {
    Clear();
}

static v3 cross(const v3& v, const v3& w) {
    const auto _v = v.data();
    const auto _w = w.data();
    return v3{
            _v[1] * _w[2] - _v[2] * _w[1],
            _v[2] * _w[0] - _v[0] * _w[2],
            _v[0] * _w[1] - _v[1] * _w[0],
    }.normalize();
}

void Acleris::LookAt(const v3& eye, const v3& center, const v3& up) {
    const v3 f = (center - eye).normalize();
    const v3 s = cross(up, f).normalize();
    const v3 u = cross(f, s);

    v4 last_col = v4{
        -s.dot(eye), -u.dot(eye), -f.dot(eye), 1
    };

    view = {
            {s.get<0>(), s.get<1>(), s.get<2>(), -s.dot(eye)},
            {u.get<0>(), u.get<1>(), u.get<2>(), -u.dot(eye)},
            {f.get<0>(), f.get<1>(), f.get<2>(), -f.dot(eye)},
            {0, 0, 0, 1},
    };

//    view = {s.extend<4>(), u.extend<4>(), f.extend<4>(), last_col};
}

void Acleris::Projection(float l, float r, float b, float t, float n, float f) {
    // see http://www.songho.ca/opengl/gl_projectionmatrix.html
    near = n;
    far = f;
    projection = {
            {(2 * n) / (r - l), 0,                 (r + l) / (r - l),   0},
            {0,                 (2 * n) / (t - b), (t + b) / (t - b),   0},
            {0,                 0,                 (-(f + n)) / (f - n), (-2 * f * n) / (f - n)},
            {0,                 0,                 -1,                  0},
    };
}

void Acleris::Projection(float r, float t, float n, float f) {
    near = n;
    far = f;
    projection = {
            {n / r, 0,     0,                    0},
            {0,     n / t, 0,                    0},
            {0,     0,     (-(f + n)) / (f - n), (-2 * f * n) / (f - n)},
            {0,     0,     -1,                   0},
    };
}

void* Acleris::SDLMakeWindow() const {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        throw std::runtime_error("Failed to initialize window");
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

    SDL_ShowCursor(0);
    SDL_SetRelativeMouseMode((SDL_bool)frontend.SDL.mouse_trapped);

    SDL_GL_SetSwapInterval(1);
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
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                bool pressed = event.key.type == SDL_KEYDOWN;
                if ((0 <= event.key.keysym.sym) && (event.key.keysym.sym < 128)) {
                    keyboard.ascii[event.key.keysym.sym] = pressed;
                }
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        keyboard.space = pressed;
                        break;
                    case SDLK_UP:
                        keyboard.up = pressed;
                        break;
                    case SDLK_DOWN:
                        keyboard.down = pressed;
                        break;
                    case SDLK_LEFT:
                        keyboard.left = pressed;
                        break;
                    case SDLK_RIGHT:
                        keyboard.right = pressed;
                        break;
                    case SDLK_LSHIFT:
                        keyboard.lshift = pressed;
                        break;
                    case SDLK_ESCAPE:
                        frontend.SDL.mouse_trapped ^= pressed;
                        SDL_SetRelativeMouseMode((SDL_bool)frontend.SDL.mouse_trapped);
                        SDL_ShowCursor(!frontend.SDL.mouse_trapped);
                        keyboard.esc = pressed;
                        break;
                    default:
                        break;
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                mouse.x += event.motion.xrel;
                mouse.y += event.motion.yrel;
                break;
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