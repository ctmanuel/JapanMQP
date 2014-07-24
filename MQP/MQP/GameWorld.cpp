#include "GameWorld.h"

using namespace C4;

GameWorld::GameWorld(const char* name) : World(name)
{
}

GameWorld::~GameWorld()
{
}

WorldResult GameWorld::Preprocess(void)
{
	WorldResult result = World::Preprocess();
	if (result != kWorldOkay)
	{
		return result;
	}
	else
	{
		SetCamera(&camera);
		return kWorldOkay;
	}
}

void GameWorld::Render(void)
{
	World::Render();
}