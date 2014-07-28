#pragma once

#include "C4Application.h"
#include "C4Engine.h"
#include "C4World.h"
#include "C4Interface.h"

#include "GameWorld.h"
#include "ResetAction.h"
#include "Tank.h"
#include "HandController.h"

using namespace C4;

class Game : public Singleton<Game>, public Application
{
private:

	ResetAction*					resetAction;

	ModelRegistration				tankModelReg;
	ControllerReg<TankController>	tankControllerReg;
	ControllerReg<HandController>	handControllerReg;

public:

	Game(void);
	~Game(void);

	static World* ConstructWorld(const char* name, void* cookie);
};

extern "C"
{
	C4MODULEEXPORT Application* ConstructApplication(void);
}