#pragma  once
#include <SDL_ttf.h>
#include <map>
#include <string>
#include "RenderManager.h"

class AssetManager {

public:
	AssetManager();

	void AddTexture(std::string textureId, const char* filePath);

	void AddFont(std::string fontId, const char* filePath, int fontSize);

	TTF_Font* GetFont(std::string fontId);

	SDL_Texture* GetTexture(std::string textureId);

	/*	Frees up memory of loaded assets  */
	void ClearAssets();

private:
	
	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, TTF_Font*> fonts;
	
};