#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "color.h"
#include "face.h"
#include "framebuffer.h"
#include "objloader.h"
#include "triangle.h"

const std::string MODEL_PATH = "assets/model.obj";
const std::string SCREENSHOT_PATH = "screenshots/render.png";

constexpr float MODEL_ROTATION_X = 25.0f;
constexpr float MODEL_ROTATION_Y = -35.0f;
constexpr float SCREEN_FILL_RATIO = 0.7f;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

std::vector<glm::vec3> vertexArray;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << '\n';
        return false;
    }

    window = SDL_CreateWindow("Software Renderer - Samuel Robledo", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == nullptr) {
        std::cerr << "Error creating window: " << SDL_GetError() << '\n';
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Error creating renderer: " << SDL_GetError() << '\n';
        return false;
    }

    return true;
}

std::vector<glm::vec3> setupVertexArray(const std::vector<glm::vec3>& vertices,
                                        const std::vector<Face>& faces) {
    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(rotation, glm::radians(MODEL_ROTATION_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotation = glm::rotate(rotation, glm::radians(MODEL_ROTATION_Y), glm::vec3(0.0f, 1.0f, 0.0f));

    std::vector<glm::vec3> rotatedVertices;
    rotatedVertices.reserve(vertices.size());
    for (const glm::vec3& vertex : vertices) {
        rotatedVertices.push_back(glm::vec3(rotation * glm::vec4(vertex, 1.0f)));
    }

    glm::vec3 minBounds = rotatedVertices.front();
    glm::vec3 maxBounds = rotatedVertices.front();
    for (const glm::vec3& vertex : rotatedVertices) {
        minBounds = glm::min(minBounds, vertex);
        maxBounds = glm::max(maxBounds, vertex);
    }

    const glm::vec3 center = (minBounds + maxBounds) * 0.5f;
    const glm::vec3 size = maxBounds - minBounds;
    const float largestDimension = std::max({size.x, size.y, size.z});
    const float targetSize = SCREEN_FILL_RATIO * static_cast<float>(std::min(SCREEN_WIDTH, SCREEN_HEIGHT));
    const float scale = largestDimension > 0.0f ? targetSize / largestDimension : 1.0f;
    const glm::vec3 screenCenter(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);

    auto toScreen = [&](const glm::vec3& vertex) {
        const glm::vec3 centered = (vertex - center) * scale;
        return glm::vec3(screenCenter.x + centered.x, screenCenter.y - centered.y, centered.z);
    };

    std::vector<glm::vec3> result;
    result.reserve(faces.size() * 3);
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];
        const int indexA = face.vertexIndices[0][0];
        const int indexB = face.vertexIndices[1][0];
        const int indexC = face.vertexIndices[2][0];

        result.push_back(toScreen(rotatedVertices[indexA]));
        result.push_back(toScreen(rotatedVertices[indexB]));
        result.push_back(toScreen(rotatedVertices[indexC]));
    }

    return result;
}

void render() {
    for (size_t i = 0; i + 2 < vertexArray.size(); i += 3) {
        triangle(vertexArray[i], vertexArray[i + 1], vertexArray[i + 2]);
    }
}

void shutdown() {
    destroyBufferTexture();
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

int main() {
    std::vector<glm::vec3> vertices;
    std::vector<Face> faces;
    if (!loadOBJ(MODEL_PATH, vertices, faces)) {
        return 1;
    }

    vertexArray = setupVertexArray(vertices, faces);

    if (!init()) {
        shutdown();
        return 1;
    }

    bool running = true;
    bool firstFrame = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_s) {
                    saveScreenshot(SCREENSHOT_PATH);
                }
            }
        }

        clear();
        setColor(Color(255, 255, 0));
        render();

        if (firstFrame) {
            saveScreenshot(SCREENSHOT_PATH);
            firstFrame = false;
        }

        renderBuffer(renderer);
        SDL_Delay(1);
    }

    shutdown();
    return 0;
}
