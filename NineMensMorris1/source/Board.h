#pragma once
#include <glm.hpp>
#include <vector>
#include <array>
#include <SDL.h>
#include <string>
#include "Piece.h"
#include "Game.h"

class AssetManager;

class Board {

  

	const int mills_col[8][3] = {
		{0, 9, 21},
		{3, 10, 18},
		{6, 11, 15},
		{8, 12, 17},
		{5, 13, 20},
		{2, 14, 23},
		{1, 4, 7},
		{16, 19, 22}
	};
	const int mills_row[8][3] = {
		{0, 1, 2},
		{3, 4, 5},
		{6, 7, 8},
		{9, 10, 11},
		{12, 13, 14},
		{15, 16, 17},
		{18, 19, 20},
		{21, 22, 23}
	};

	/*	Positions 0-23  */
	const std::vector<std::vector<int>> neighbours = {
		{1,9},			
		{0,2,4},		
		{1,14},			
		{4,10},			
		{1,3,5,7},		
		{4,13},			
		{7,11},			
		{4,6,8},		
		{7,12},			
		{0,10,21},		
		{3,9,11,18},	
		{6,10,15},		
		{8,13,17},		
		{5,12,14,20},	
		{2,13,23},		
		{11,16},		
		{15,17,19},		
		{12,16},	
		{10,19},		
		{16,18,20,22},	
		{19,13},		
		{9,22},			
		{19,21,23},		
		{14,22}			
	};


public:

    std::array<Piece*, 24> boardState;
    std::vector<int> markers;
    int movingPiece;
	bool gameOver = false;
	bool isAnimating = false;

	std::string playerTextures[2];
	std::string playerTexturesWing[2];


    Board();
    ~Board();

    /*	Returns window coordinates of positions 0-23 */
    glm::vec2 GetBoardPosition(int pos) const;

	/*	Returns board positions from  mouse click coordinates */
    int GetPositionOnBoard(int x, int y) const;

	/*	Returns empty board positions */
	std::vector<int> GetEmptyPositions() const;

	/*	Returns amount of pieces left in "hand" */
	int GetPlayerPiecesCount(Game::PlayerTurn turn) const;

	/*	Returns amount of pieces left on the board */
	int GetPlayerPiecesOnBoardCount(Game::PlayerTurn turn) const;

	/*	Returns true if a player has 2 pieces left on  the board OR no legal moves */
	bool IsGameOver(Game::PlayerTurn turn) const;

	/*	Removes a piece at a given position */
	void RemoveAt(int pos);

	/*	Places a piece at a given position */
	bool SetPiece(int pos, Game::PlayerTurn turn);

	/*	Sets colored markers around positions */
	void SetMarkers(std::vector<int> positions);

	/*	Clears informational markers */
	void ClearMarkers();

	void AnimatePieces();

	/*	Returns true if a given position is in mill */
	bool CheckIfMill(int position);
	
	/* 
	*	Returns a vector of positions that can be removed 
	*	Prioritises positions that are not in a mill
	*/
	std::vector<int> GetRemoveablePieces(Game::PlayerTurn turn);

	/*	Returns a vector of positions  */
	std::vector<int> GetMoveablePositions(int position, Game::PlayerTurn turn) const;

	/*	Returns positions of pieces that have legal moves	*/
	std::vector<int> GetPiecesThatCanMove(Game::PlayerTurn turn);

	/* Returns true if pieces on given positions have the same owner */
	bool IsSameOwner(int p1, int p2, int p3);

	/* 
	*	Changes texture to @textureId on all pieces from a given player
	*/
	void SetTextureOnPieces(std::string textureId, Game::PlayerTurn);

	/*	Renders current game state */
    void RenderState(Game::PlayerTurn turn, Game::State gameState);

private:

    SDL_Rect boardRect;
    const int maxPieces = 9;
    std::vector<glm::vec2> boardPositions;
	std::vector<Piece*> playerPieces_1;
	std::vector<Piece*> playerPieces_2;




	/*	Returns true if point (x,y) is in bounds of one of the positions on the board */
	bool IsInBounds(int position, int x, int y) const;

    void InitializePositions();

    void InitializePieces();

	void InitializeMarkers();

	void DrawMarkers();

	void DrawPiecesLeft();

	int Lerp(int v1, int v2, float t);

	void DrawGameOverMesage(Game::PlayerTurn player);
};




