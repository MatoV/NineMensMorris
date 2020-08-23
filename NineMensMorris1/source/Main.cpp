#include <glm.hpp>
#include "Game.h"
#include "Constants.h"

int main(int argc, char* argv[]) {
	
	Game* game = new Game();

	game->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

	/* Main loop*/
	while (game->IsRunning()) {
		game->Update();
		game->Render();
	}
	delete game;
	return 0;
}