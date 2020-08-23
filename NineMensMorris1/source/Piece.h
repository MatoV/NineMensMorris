#pragma once
#include <SDL.h>
#include <string>

class Piece {

public:
	
	int owner;
	int posX, posY;
	int targX, targY;
	bool isAnimating;

	Piece(const char* textureId, int owner);

	void SetTexture(std::string assetTextureId);
	
	void Draw(int x, int y, int alpha = 255);


private:

	SDL_Rect destRect;
	SDL_Texture* texture;
	int width, height;
};