
#include "RenderManager.h"
#include <SDL_image.h>
#include "Game.h"
#include "AssetManager.h"

SDL_Texture* RenderManager::LoadTexture(const char* fileName) {
	SDL_Surface* surface = IMG_Load(fileName);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer,surface);
	SDL_FreeSurface(surface);
	return texture;
}

TTF_Font* RenderManager::LoadFont(const char* fileName, int fontSize) {
	return TTF_OpenFont(fileName, fontSize);
}

void RenderManager::DrawText(std::string text, int x, int y, const SDL_Color& color, std::string fontId) {

	SDL_Surface* surface = TTF_RenderText_Blended(Game::assetManager->GetFont(fontId), text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
	SDL_Rect textRect;
	textRect.x = x;
	textRect.y = y;
	SDL_FreeSurface(surface);
	SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
	SDL_RenderCopy(Game::renderer, texture, NULL, &textRect);
	SDL_DestroyTexture(texture);
}

void RenderManager::Draw(SDL_Texture* texture, SDL_Rect destinationRectangle, int alpha, SDL_RendererFlip flip) {
	SDL_SetTextureAlphaMod(texture, alpha);
	SDL_RenderCopyEx(Game::renderer, texture, NULL, &destinationRectangle, 0.0f, NULL, flip);
}
