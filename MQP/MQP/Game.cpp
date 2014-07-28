#include "Game.h"

using namespace C4;

Game* TheGame = nullptr;

Application* ConstructApplication(void)
{
	return (new Game);
}

Game::Game() : 
	Singleton<Game>(TheGame),
	tankControllerReg(kControllerTank, "Tank"),										//register the tank controller
	tankModelReg(kModelTank, "Tank", "tank", kModelPrecache, kControllerTank),		//register the tank model
	playerControllerReg(kControllerPlayer, "Player")
{
	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
	resetAction = new ResetAction(kActionReset);
	TheInputMgr->AddAction(resetAction);
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
