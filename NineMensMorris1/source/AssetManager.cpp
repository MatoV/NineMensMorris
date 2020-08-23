#include "AssetManager.h"

AssetManager::AssetManager() {}

void AssetManager::AddTexture(std::string textureId, const char* filePath) {
	textures.emplace(textureId, RenderManager::LoadTexture(filePath));
}

void AssetManager::AddFont(std::string fontId, const char* filePath, int fontSize) {
	fonts.emplace(fontId, RenderManager::LoadFont(filePath, fontSize));
}

TTF_Font* AssetManager::GetFont(std::string fontId) {
	return fonts[fontId];
}

SDL_Texture* AssetManager::GetTexture(std::string textureId) {
	return textures[textureId];
}

void AssetManager::ClearAssets() {
	for (auto& f : fonts) {
		TTF_CloseFont(f.second);
	}
	for (auto& t : textures) {
		SDL_DestroyTexture(t.second);
	}
}
