#include "framebuffer.h"

#include <filesystem>
#include <iostream>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

std::vector<Color> framebuffer(SCREEN_WIDTH * SCREEN_HEIGHT);
Color currentColor(255, 255, 255);
Color clearColor(18, 18, 28);

static SDL_Texture* bufferTexture = nullptr;

void setColor(const Color& color) {
    currentColor = color;
}

void clear() {
    std::fill(framebuffer.begin(), framebuffer.end(), clearColor);
}

void point(int x, int y) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    framebuffer[y * SCREEN_WIDTH + x] = currentColor;
}

void renderBuffer(SDL_Renderer* renderer) {
    if (bufferTexture == nullptr) {
        bufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
                                          SCREEN_WIDTH, SCREEN_HEIGHT);
        if (bufferTexture == nullptr) {
            std::cerr << "Error creating texture: " << SDL_GetError() << '\n';
            return;
        }
    }

    SDL_UpdateTexture(bufferTexture, nullptr, framebuffer.data(), SCREEN_WIDTH * sizeof(Color));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bufferTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void destroyBufferTexture() {
    if (bufferTexture != nullptr) {
        SDL_DestroyTexture(bufferTexture);
        bufferTexture = nullptr;
    }
}

bool saveScreenshot(const std::string& path) {
    const std::filesystem::path directory = std::filesystem::path(path).parent_path();
    if (!directory.empty()) {
        std::error_code error;
        std::filesystem::create_directories(directory, error);
    }

    const int stride = SCREEN_WIDTH * static_cast<int>(sizeof(Color));
    if (stbi_write_png(path.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, 3, framebuffer.data(), stride) == 0) {
        std::cerr << "Error saving screenshot: " << path << '\n';
        return false;
    }

    std::cout << "Screenshot saved: " << path << '\n';
    return true;
}
