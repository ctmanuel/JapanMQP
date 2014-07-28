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
	handControllerReg(kControllerHand, "Hand"),
	tankModelReg(kModelTank, "Tank", "tank", kModelPrecache, kControllerTank)
{
	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
	resetAction = new ResetAction(kActionReset);
	TheInputMgr->AddAction(resetAction);
	TheWorldMgr->LoadWorld("GameWorld_01");
}

Game::~Game()
{
	TheWorldMgr->UnloadWorld();
	TheWorldMgr->SetWorldConstructor(nullptr);

	delete resetAction;
}

World* Game::ConstructWorld(const char* name, void* cookie)
{
	return (new GameWorld(name));
}
