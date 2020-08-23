#include "Piece.h"
#include "Game.h"
#include "AssetManager.h"
#include <iostream>



Piece::Piece(const char* textureId, int owner) {
	this->destRect.x = 0;
	this->destRect.y = 0;
	this->posX = 0;
	this->posY = 0;
	this->targX = 0;
	this->targY = 0;
	this->owner = owner;
	this->isAnimating = false;
	SetTexture(textureId);
}

void Piece::SetTexture(std::string assetTextureId) {
	int w, h;
	texture = Game::assetManager->GetTexture(assetTextureId);
	SDL_QueryTexture(texture,NULL,NULL, &w, &h);
	destRect.w = w;
	destRect.h = h;
	width = w;
	height = h;
}

void Piece::Draw(int x, int y, int alpha) {
	destRect.x = x - width/2;
	destRect.y = y - height/2;
	if (!texture) {
		std::cerr << "Piece::Texture is a nullptr \n";
	}
	RenderManager::Draw(texture, destRect, alpha);
}
