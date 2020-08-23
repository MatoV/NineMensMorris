#include "Board.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "Constants.h"

Board::Board() {


	this->movingPiece = -1;
	this->playerTextures[0] = "figureWhite-image";
	this->playerTexturesWing[0] = "figureWhiteWing-image";
	this->playerTextures[1] = "figureBlack-image";
	this->playerTexturesWing[1] = "figureBlackWing-image";

	this->boardRect.x = 0;
	this->boardRect.y = 0;
	this->boardRect.w = 818;
	this->boardRect.h = 818;
	
	InitializePositions();
	InitializePieces();
	InitializeMarkers();


}

Board::~Board() {
	for (auto p : boardState) {
		if (p != nullptr)
			delete p;
	}
	for (auto p : playerPieces_1) {
		if (p != nullptr)
			delete p;
	}
	for (auto p : playerPieces_2) {
		if (p != nullptr)
			delete p;
	}
}

glm::vec2 Board::GetBoardPosition(int pos) const {

	if (pos >= 0 && pos <= 23) {
		return boardPositions[pos];
	}
	return glm::vec2(0);
}

int Board::GetPositionOnBoard(int x, int y) const {
	for (int i = 0; i < boardState.size(); i++) {
			if (IsInBounds(i, x, y)) {
				return i;
			}
	}
	return -1;
}

std::vector<int> Board::GetEmptyPositions() const {
	std::vector<int> positions;
	for (int i = 0; i < boardState.size(); i++) {
		if (!boardState[i]) {
			positions.push_back(i);
		}
	}
	return positions;
}

int Board::GetPlayerPiecesCount(Game::PlayerTurn player) const {
	if (player == Game::PlayerTurn::PLAYER1) {
		return playerPieces_1.size();
	}
	else {
		return playerPieces_2.size();
	}
}

int Board::GetPlayerPiecesOnBoardCount(Game::PlayerTurn player) const {
	int playerPieces = 0;
	int owner = static_cast<int>(player);

	for (auto const& p : boardState) {
		if (p && p->owner == owner) {
			playerPieces++;
		}
	}
	return playerPieces;
}

bool Board::IsGameOver(Game::PlayerTurn player) const {

	if (GetPlayerPiecesOnBoardCount(player) <= 2) { 
		return true; 
	}

	int owner = static_cast<int>(player);
	
	/* Check if there's legal moves left */
	for (int i = 0; i < boardState.size(); i++) {
		Piece* p = boardState[i]; 
		if (p && p->owner == owner && GetMoveablePositions(i, player).size() > 0) {
			return false;
		}
	}
	return true;
}

void Board::RemoveAt(int pos) {
	Piece* p = boardState[pos];
	delete p;
	boardState[pos] = nullptr;
}

bool Board::SetPiece(int pos, Game::PlayerTurn player) {
	if (boardState[pos] == nullptr) {
		if (player == Game::PlayerTurn::PLAYER1) {
			if (playerPieces_1.size() > 0) {
				boardState[pos] = playerPieces_1[0];
				playerPieces_1.erase(playerPieces_1.begin());
				return true;
			}
		}
		else {
			if (playerPieces_2.size() > 0) {
				boardState[pos] = playerPieces_2[0];
				playerPieces_2.erase(playerPieces_2.begin());
				return true;
			}
		}
	}
	return false;
}

void Board::SetMarkers(std::vector<int> positions) {
	for (auto& i : positions) {
		markers[i] = 1;
	}
}

void Board::ClearMarkers() {
	for (int i = 0; i < 24; i++) {
		markers[i] = -1;
	}
}

void Board::AnimatePieces() {

	for (auto& p : boardState) {
		if (p != nullptr && p->isAnimating) {
			int x, y;
			if (glm::abs(p->posX - p->targX) + glm::abs(p->posY- p->targY) <= 9) {
				p->posX = p->targX;
				p->posY = p->targY;
				p->isAnimating = false;
				this->isAnimating = false;
				break;
			}
			x = Lerp(p->posX, p->targX, 0.2);
			y = Lerp(p->posY, p->targY, 0.2);
			p->posX = x;
			p->posY = y;
			
		}
	}
}

bool Board::CheckIfMill(int position) {
	for (auto p : mills_col) {
		for (int i = 0; i < 3; i++) {
			if (p[i] == position) {
				if (IsSameOwner(p[0], p[1], p[2]))
					return true;
			}
		}
	}
	for (auto item : mills_row) {
		for (int i = 0; i < 3; i++) {
			if (item[i] == position) {
				if (IsSameOwner(item[0], item[1], item[2]))
					return true;
			}
		}
	}
	return false;
}

std::vector<int> Board::GetRemoveablePieces(Game::PlayerTurn player)  {

	std::vector<int> noMills, mills;
	int enemy = !static_cast<int>(player);
	for (int i = 0; i < 24; i++) {
		if (boardState[i] && boardState[i]->owner == enemy) {
			if (CheckIfMill(i)) {
				if (!(std::find(mills.begin(), mills.end(), i) != mills.end()))
					mills.push_back(i);
			}
			else {
				if (!(std::find(noMills.begin(), noMills.end(), i) != noMills.end()))
					noMills.push_back(i);
			}
		}
	}

	return noMills.size() != 0 ? noMills : mills;
}

std::vector<int> Board::GetMoveablePositions(int position, Game::PlayerTurn player) const {

	std::vector<int> moveables;
	int piecesLeft = GetPlayerPiecesOnBoardCount(player);
	/* If 3 pieces left, we can "fly" */
	if (piecesLeft == 3) {
		moveables = GetEmptyPositions();
	}
	else {
	/* Check for neighbours*/
		for (auto& item : neighbours[position]) {
			if (boardState[item] == nullptr) {
				moveables.push_back(item);
			}
		}
	}
	return moveables;
}

std::vector<int> Board::GetPiecesThatCanMove(Game::PlayerTurn player) {

	std::vector<int> piecePositions;

	int owner = static_cast<int>(player);

	/* Find pieces that can move */
	for (int i = 0; i < boardState.size(); i++) {
		Piece* p = boardState[i];
		if (p && p->owner == owner && GetMoveablePositions(i, player).size() > 0) {
			piecePositions.push_back(i);
		}
	}
	return piecePositions;
}

bool Board::IsSameOwner(int p1, int p2, int p3) {

	if (boardState[p1] && boardState[p2] && boardState[p3]
		&& (boardState[p1]->owner == boardState[p2]->owner && boardState[p1]->owner == boardState[p3]->owner)) {
		return true;
	}
	return false;
}

void Board::SetTextureOnPieces(std::string textureId, Game::PlayerTurn player) {
	for (auto& s : boardState) {
		if (s != nullptr && s->owner == static_cast<int>(player)) {
			s->SetTexture(textureId);
		}
	}

}

void Board::RenderState(Game::PlayerTurn turn, Game::State gameState) {
	
	
	/* Draw pieces on board */
	RenderManager::Draw(Game::assetManager->GetTexture("board-image"), boardRect);

	for (int i = 0; i < boardState.size(); i++) {
		Piece* p = boardState[i];
		if (p != nullptr) {
			glm::vec2 pos = GetBoardPosition(i);
			if (p->isAnimating) {
				p->Draw(p->posX, p->posY);
			}
			else {
				p->Draw(pos.x, pos.y);
			}
		}
	}

	DrawMarkers();

	DrawPiecesLeft();

	/* Drawing informational text */

	std::string text, playerText;
	SDL_Color textColor, playerColor;
	if (turn == Game::PlayerTurn::PLAYER1) {
		playerText = "Player 1";
		playerColor = { 59, 179, 175 };
	}
	else {
		playerText = "Player 2";
		playerColor = { 179, 101, 59 };
	}
	

	switch (gameState) {
	case Game::State::MOVE:
		textColor = { 48, 130, 61 };
		text += ": Move a piece";
		break;
	case Game::State::PLACE:
		textColor = { 29, 29, 143 };
		text += ": Place a piece";
		break;
	case Game::State::MILL:
		textColor = { 143, 29, 29 };
		text += ": Remove a piece";
		break;
	}
	RenderManager::DrawText( playerText, 150, 0, playerColor, "charriotBig-font");
	RenderManager::DrawText( text, 290, 0, textColor, "charriotBig-font");


	RenderManager::DrawText("(Press R to restart the game)", 50, 800, { 0,0,0 }, "charriotSmall-font");
	

	if (gameOver) {
		DrawGameOverMesage(turn);
	}
}



bool Board::IsInBounds(int pos, int x, int y) const 
{
	int x1, y1, x2, y2;
	glm::vec2 coord = GetBoardPosition(pos);
	x1 = coord.x - 24;
	y1 = coord.y - 24;
	x2 = coord.x + 24;
	y2 = coord.y + 24;

	if (x > x1 && x < x2 && y > y1 && y < y2) return true;
	return false;
}

void Board::InitializePositions() {
	boardPositions.push_back(glm::vec2(60, 60));
	boardPositions.push_back(glm::vec2(410, 60));
	boardPositions.push_back(glm::vec2(760, 60));

	boardPositions.push_back(glm::vec2(175, 175));
	boardPositions.push_back(glm::vec2(410, 175));
	boardPositions.push_back(glm::vec2(642, 175));
	
	boardPositions.push_back(glm::vec2(292, 292));
	boardPositions.push_back(glm::vec2(410, 292));
	boardPositions.push_back(glm::vec2(525, 292));
	
	boardPositions.push_back(glm::vec2(60, 410));
	boardPositions.push_back(glm::vec2(175, 410));
	boardPositions.push_back(glm::vec2(292, 410));
	boardPositions.push_back(glm::vec2(525, 410));
	boardPositions.push_back(glm::vec2(642, 410));
	boardPositions.push_back(glm::vec2(760, 410));

	boardPositions.push_back(glm::vec2(292, 525));
	boardPositions.push_back(glm::vec2(410, 525));
	boardPositions.push_back(glm::vec2(525, 525));
	
	boardPositions.push_back(glm::vec2(175, 642));
	boardPositions.push_back(glm::vec2(410, 642));
	boardPositions.push_back(glm::vec2(642, 642));
	
	boardPositions.push_back(glm::vec2(60, 760));
	boardPositions.push_back(glm::vec2(410, 760));
	boardPositions.push_back(glm::vec2(760, 760));




}

void Board::InitializePieces() {
	for (int i = 0; i < maxPieces; i++) {
		playerPieces_1.push_back(new Piece(playerTextures[0].c_str(), 0));
		playerPieces_2.push_back(new Piece(playerTextures[1].c_str(), 1));
	}
}

void Board::InitializeMarkers() {

	for (int i = 0; i < 24; i++) {
		markers.push_back(-1);
	}
}

void Board::DrawMarkers() {
	SDL_Rect r;
	for (int i = 0; i < 24; i++) {
		if (markers[i] != -1) {
			glm::vec2 pos = GetBoardPosition(i);
			r.x = pos.x - 30;
			r.y = pos.y - 30;
			r.w = 60;
			r.h = 60;
			RenderManager::Draw(Game::assetManager->GetTexture("figureMarker-image"), r, 150);
		}
	}

	if (movingPiece != -1) {
		SDL_Rect r;
		glm::vec2 pos = GetBoardPosition(movingPiece);
		r.x = pos.x - 30;
		r.y = pos.y - 30;
		r.w = 60;
		r.h = 60;
		RenderManager::Draw(Game::assetManager->GetTexture("figureMoveMarker-image"), r, 150);
	}
}

void Board::DrawPiecesLeft() {
	SDL_Rect r;
	SDL_Color c{ 0, 0, 0, 0 };

	int p1 = 0, p2 = 0;
	std::string s1, s2, message = "Pieces left to place";

	p1 = GetPlayerPiecesCount(Game::PlayerTurn::PLAYER1);
	p2 = GetPlayerPiecesCount(Game::PlayerTurn::PLAYER2);
	if (p1 == 0 && p2 == 0) {
		p1 = GetPlayerPiecesOnBoardCount(Game::PlayerTurn::PLAYER1);
		p2 = GetPlayerPiecesOnBoardCount(Game::PlayerTurn::PLAYER2);
		message = "Pieces left on board";
	}
	s1 = std::to_string(p1);
	s2 = std::to_string(p2);
	
	RenderManager::DrawText(message, 768, 85, c);
	RenderManager::DrawText("Player 1: ", 770, 120, c, "charriotSmall-font");
	RenderManager::DrawText("Player 2: ", 870, 120, c, "charriotSmall-font");
	c = { 255,0,0 };
	RenderManager::DrawText(s1, 845, 120, c ,"charriotSmall-font");
	RenderManager::DrawText(s2, 950, 120, c, "charriotSmall-font");


	for (int i = 0; i < p1; i++) {
		r.x = 800;
		r.y = 150 + i *13;
		r.w = 32;
		r.h = 32;
		RenderManager::Draw(Game::assetManager->GetTexture(playerTextures[0]), r, 255);
	}
	for (int i = 0; i < p2; i++) {
		r.x = 900;
		r.y = 150 + i *13;
		r.w = 32;
		r.h = 32;
		RenderManager::Draw(Game::assetManager->GetTexture(playerTextures[1]), r, 255);
	}
}

int Board::Lerp(int v1, int v2, float t) {

	return ((1 - t) * v1 + t * v2);
}

void Board::DrawGameOverMesage(Game::PlayerTurn player) {
	SDL_Color c{ 255,255, 255 };
	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = WINDOW_WIDTH;
	r.h = WINDOW_HEIGHT;
	RenderManager::Draw(Game::assetManager->GetTexture("gameOver-image"), r, 255);
	std::string gameOverMessage;
	std::string playerText;
	if (player == Game::PlayerTurn::PLAYER1) {
		playerText = "Player 1 ";
	}
	else {
		playerText = "Player 2 ";
	}
	gameOverMessage = playerText + " WON!";

	RenderManager::DrawText("GAME OVER", 350, 350, c, "audiowide-font");
	RenderManager::DrawText(gameOverMessage, 350, 400, c, "audiowide-font");
	RenderManager::DrawText("Press SPACE to restart the game or ESC to exit.", 50, 450,c, "audiowide-font");
}
