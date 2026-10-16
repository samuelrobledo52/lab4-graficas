#pragma once

#include <string>
#include <vector>

#include <SDL.h>

#include "color.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

extern std::vector<Color> framebuffer;
extern Color currentColor;
extern Color clearColor;

void setColor(const Color& color);
void clear();
void point(int x, int y);
void renderBuffer(SDL_Renderer* renderer);
void destroyBufferTexture();
bool saveScreenshot(const std::string& path);
