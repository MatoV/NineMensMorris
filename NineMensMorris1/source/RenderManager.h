#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>


class RenderManager {

public:
	static SDL_Texture* LoadTexture(const char* fileName);

	static TTF_Font* LoadFont(const char* fileName, int fontSize);

	static void DrawText(std::string text, int x, int y, const SDL_Color& color = { 255, 0, 0 }, std::string fontId = "charriot-font");

	static void Draw(SDL_Texture* texture, SDL_Rect destinationRectangle, int alpha = 255, SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE);

};