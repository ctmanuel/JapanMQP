#include "Game.h"

using namespace C4;

Game* TheGame = nullptr;

Application* ConstructApplication(void)
{
	return (new Game);
}

Game::Game() : 
	Singleton<Game>(TheGame),
	tankControllerReg(kControllerTank, "Tank"),
	tankModelReg(kModelTank, "Tank", "tank", kModelPrecache, kControllerTank)
{
	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
}

Game::~Game()
{
	TheWorldMgr->UnloadWorld();
	TheWorldMgr->SetWorldConstructor(nullptr);
}

World* Game::ConstructWorld(const char* name, void* cookie)
{
	return (new GameWorld(name));
}
