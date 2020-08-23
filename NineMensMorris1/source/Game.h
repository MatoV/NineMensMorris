#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Piece.h"
#include <glm.hpp>

class Board;
class AssetManager;
class SDL_MouseButtonEvent;


class Game {

public:
	enum class PlayerTurn : int {
		PLAYER1,
		PLAYER2
	};

	enum class State {
		PLACE,
		MOVE,
		MILL,
		GAMEOVER
	};

	Game();
	~Game();

	int ticksLastFrame;

	State gameState;

	PlayerTurn playerTurn;

	static AssetManager* assetManager;

	static SDL_Renderer* renderer;

	static SDL_Event event;

	
	void Initialize(int width, int height);
	
	void Reset();

	/*	Updates game state	*/
	void Update();
	
	/*	Renders game state	*/
	void Render();
	
	bool IsRunning() const;

private:
	bool isRunning;
	Board* board;
	SDL_Window* window = nullptr;

	/*
	*	 Processes player input and changes state based on it
	*/
	void ProcessInput();

	int GetClickPosition(SDL_MouseButtonEvent buttonEvent);

	void HandleMilling(int position);

	void HandlePlacing(int position);

	void HandleMoving(int position);

};
