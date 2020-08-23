#include "Game.h"
#include <iostream>
#include <SDL_ttf.h>
#include <SDL_image.h>	
#include "Constants.h"
#include "RenderManager.h"
#include "Board.h"
#include "AssetManager.h"


SDL_Renderer* Game::renderer;
SDL_Event Game::event;
AssetManager* Game::assetManager = new AssetManager();



Game::Game() {
	this->isRunning = false;
	this->ticksLastFrame = 0;
	/* Init starting game state */
	this->gameState = State::PLACE;
	this->playerTurn = PlayerTurn::PLAYER1;
}

Game::~Game() {
	/* Release allocated memory */
	assetManager->ClearAssets();
	delete board;
	delete assetManager;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}


bool Game::IsRunning() const {
	return this->isRunning;
}

int Game::GetClickPosition(SDL_MouseButtonEvent buttonEvent) {
	int pos = board->GetPositionOnBoard(buttonEvent.x, buttonEvent.y);
	return pos;
}

void Game::HandleMilling(int position) {
	/* Find and remove selected piece */
	if (board->markers[position] == 1) {
		board->RemoveAt(position);
		board->ClearMarkers();
		
		if (board->GetPlayerPiecesCount(PlayerTurn::PLAYER2) == 0) {
			/* Test for Game Over state */
			PlayerTurn enemy = static_cast<PlayerTurn>(!static_cast<int>(playerTurn));
			bool isOver = board->IsGameOver(enemy);
			if (isOver) {
				gameState = State::GAMEOVER;
				board->gameOver = true;
				return;
			}
			else {
				gameState = State::MOVE;
			}
		}
		else {
			gameState = State::PLACE;
		}
		/* Change textures if player can "fly" */
		playerTurn = static_cast<PlayerTurn>(!static_cast<int>(playerTurn));
		if (board->GetPlayerPiecesCount(playerTurn) == 0 && board->GetPlayerPiecesOnBoardCount(playerTurn) == 3) {
			board->SetTextureOnPieces(board->playerTexturesWing[static_cast<int>(playerTurn)], playerTurn);
		}

		SDL_Delay(150);
	}

}

void Game::HandlePlacing(int position) {
	
	if (board->SetPiece(position, playerTurn)) {
		/* Test for mill*/
		if (board->CheckIfMill(position)) {
			/* Return pieces that can be removed, current player continues */
			gameState = State::MILL;
			board->SetMarkers(board->GetRemoveablePieces(playerTurn));
			return;
		}
		/* If second player has no more pieces to place, change state to Move */
		if (board->GetPlayerPiecesCount(PlayerTurn::PLAYER2) == 0) {
			gameState = State::MOVE;
		}

		playerTurn = static_cast<PlayerTurn>(!static_cast<int>(playerTurn));

	}
	
}

void Game::HandleMoving(int position) {

	Piece* p = board->boardState[position];
	
	/* Select piece */
	std::vector<int> moveableSpots = board->GetMoveablePositions(position, playerTurn);
	if (p != nullptr && p->owner == static_cast<int>(playerTurn) && moveableSpots.size() > 0) {
		board->movingPiece = position;
		board->ClearMarkers();
		board->SetMarkers(moveableSpots);
	}

	/* Move selected */
	if (board->markers[position] == 1) {
		/* Start animating  */
		Piece* p = board->boardState[board->movingPiece];
		glm::vec2 pos = board->GetBoardPosition(board->movingPiece);
		glm::vec2 targPos = board->GetBoardPosition(position);
		p->posX = pos.x;
		p->posY = pos.y;
		p->targX = targPos.x;
		p->targY = targPos.y;
		p->isAnimating = true;
		board->isAnimating = true;

		board->boardState[position] = board->boardState[board->movingPiece];
		board->boardState[board->movingPiece] = nullptr;
		board->movingPiece = -1;
		board->ClearMarkers();

		/* Check for  mill */
		if (board->CheckIfMill(position)) {
			board->SetMarkers(board->GetRemoveablePieces(playerTurn));
			gameState = State::MILL;
			return;
		}

		playerTurn = static_cast<PlayerTurn>(!static_cast<int>(playerTurn));
	}

}


void Game::Initialize(int width, int height) {

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Error initializing SDL." << std::endl;
		return;
	}

	/* Initialize TTF for Fonts */
	if (TTF_Init() != 0) {
		std::cerr << "Error initializing SDL_TTF." << std::endl;
		return;
	}

	/* Create main window */
	window = SDL_CreateWindow("Nine men's morris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, NULL);
	if (!window) {
		std::cerr << "Error initializing SDL Window." << std::endl;
		return;
	}
	/* Create renderer */
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		std::cerr << "Error initializing SDL renderer." << std::endl;
		return;
	}

	/* Load assets */
	assetManager->AddTexture("board-image", "./assets/images/board.png");
	assetManager->AddTexture("figureWhite-image", "./assets/images/figureWhite.png");
	assetManager->AddTexture("figureWhiteWing-image", "./assets/images/figureWhiteWing.png");
	assetManager->AddTexture("figureBlack-image", "./assets/images/figureBlack.png");
	assetManager->AddTexture("figureBlackWing-image", "./assets/images/figureBlackWing.png");
	assetManager->AddTexture("figureMarker-image", "./assets/images/figureMarker1.png");
	assetManager->AddTexture("figureMoveMarker-image", "./assets/images/figureMarker2.png");
	assetManager->AddTexture("gameOver-image", "./assets/images/background.jpg");
	assetManager->AddFont("charriot-font", "./assets/fonts/HighlandGothicFLF.ttf", 22);
	assetManager->AddFont("charriotBig-font", "./assets/fonts/HighlandGothicFLF.ttf", 30);
	assetManager->AddFont("charriotSmall-font", "./assets/fonts/HighlandGothicFLF.ttf", 16);
	assetManager->AddFont("audiowide-font", "./assets/fonts/Audiowide-Regular.ttf", 34);

	/* Initialize board*/
	board = new Board();


	isRunning = true;
}

void Game::Reset() {
	delete board;
	board = new Board();
	gameState = State::PLACE;
	playerTurn = PlayerTurn::PLAYER1;
}

void Game::ProcessInput() {
	SDL_PollEvent(&event);

	switch (event.type) {

	case SDL_QUIT:
		isRunning = false;
		SDL_Quit();
		break;
	case SDL_KEYUP:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			isRunning = false;
			SDL_Quit();
		}
		else if (event.key.keysym.sym == SDLK_SPACE && gameState == State::GAMEOVER) {
			Reset();
		}	
		else if (event.key.keysym.sym == SDLK_r) {
			Reset();
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {

				int position = GetClickPosition(event.button);

				if (position != -1) {
					switch (gameState) {
					case State::PLACE:
						HandlePlacing(position);
						break;
					case State::MOVE:
						HandleMoving(position);
						break;
					case State::MILL:
						HandleMilling(position);
						break;
					}
				}
			}
		break;
	default:
		break;
	}
}

void Game::Update() {
	int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - ticksLastFrame);
	/* Wait until we have reached our target  (60 FPS) */
	if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
		SDL_Delay(timeToWait);
	}
	/* Delta time is difference in ticks from last frame converted to seconds */
	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	/* Clamp Delta Time to a maximum value */
	deltaTime = (deltaTime > 0.05f) ? 0.0f : deltaTime;

	
	
	/* UPDATE 	*/

	/* Wait for animation to end */
	if (board->isAnimating) {
		board->AnimatePieces();
	}
	else {
		ProcessInput();
	}


	/* Sets the new ticks for current frame to be used in the next pass */
	ticksLastFrame = SDL_GetTicks();

}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 224, 224, 224, 255);
	SDL_RenderClear(renderer);

	/* Render board state */
	board->RenderState(playerTurn, gameState);

	SDL_RenderPresent(renderer);
}

