#pragma once

#include "C4Application.h"
#include "C4Engine.h"
#include "C4World.h"
#include "GameWorld.h"
#include "Tank.h"

using namespace C4;

class Game : public Singleton<Game>, public Application
{
private:

	ModelRegistration				tankModelReg;
	ControllerReg<TankController>	tankControllerReg;

public:

	Game(void);
	~Game(void);

	static World* ConstructWorld(const char* name, void* cookie);
};

extern "C"
{
	C4MODULEEXPORT Application* ConstructApplication(void);
}