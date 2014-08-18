#include "Game.h"
#include "HandController.h"

using namespace C4;

Game* TheGame = nullptr;

Application* ConstructApplication(void)
{
	return (new Game);
}

Game::Game() :
	Singleton<Game>(TheGame),														//register the player controller
	handControllerReg(kControllerAnimatedHand, "Hand"),								//register hand controller
	tankControllerReg(kControllerTank, "Tank"),										//register the tank controller
	tankModelReg(kModelTank, "Tank", "tank", kModelPrecache, kControllerTank),		//register the tank model
	gauntletModelReg(kModelAnimatedHand, "AnimatedGauntlet", "Model/gauntletAnimated", kModelPrecache, kControllerAnimatedHand),
	playerControllerReg(kControllerPlayer, "Main Player Controller"),
	lightPathControllerReg(kControllerLightPath, "Light Path"),
	playerModelReg(kModelPlayer, "Player", "player", kModelPrecache, kControllerPlayer),
	animatedHand(kLocatorAnimatedObject, "AnimatedGauntlet"),
	lightParticleSystemReg(kParticleSystemLight, "Light")
{
	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
	resetAction = new ResetAction(kActionReset);
	TheInputMgr->AddAction(resetAction);

//	LoadWorld("GameWorld_01");
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
