#pragma once

#include "C4Application.h"
#include "C4Engine.h"
#include "C4World.h"
#include "C4Interface.h"

#include "GameWorld.h"
#include "ResetAction.h"
#include "Tank.h"
#include "HandController.h"
#include "Player.h"
#include "LightPathController.h"
#include "LightParticleSystem.h"

using namespace C4;
enum {

	kLocatorAnimatedObject = 'aobj'
};

class Game : public Singleton<Game>, public Application
{
private:

	DisplayEventHandler				displayEventHandler;
	ResetAction*							resetAction;

	ModelRegistration						playerModelReg;
	ModelRegistration						gauntletModelReg;

	ControllerReg<HandController>			handControllerReg;
	ControllerReg<MainPlayerController>		playerControllerReg;
	ControllerReg<LightPathController>		lightPathControllerReg;

	LocatorRegistration						animatedHand;

	ParticleSystemReg<LightParticleSystem>	lightParticleSystemReg;

	HandController							*handController;

	static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);

public:

	Game(void);
	~Game(void);

	static World* ConstructWorld(const char* name, void* cookie);
	HandController *GetHandController(void) const
	{
		return (handController);
	}
	EngineResult LoadWorld(const char *name) override;
	void UnloadWorld(void) override;
};

extern "C"
{
	C4MODULEEXPORT Application* ConstructApplication(void);
}

extern Game *TheGame;
