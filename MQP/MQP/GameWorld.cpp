#include "GameWorld.h"
#include "Game.h"
#include "HandController.h"
#include "C4Controller.h"

using namespace C4;

GameWorld::GameWorld(const char* name) : World(name)
{
}

GameWorld::~GameWorld()
{
	music->Stop();
	music->Release();
}

WorldResult GameWorld::Preprocess(void)
{
	//The Preprocess() function is called after the world has been constructed.
	//We always call the base PReprocess() function first. If it returns an error
	//then return the same result code.

	WorldResult result = World::Preprocess();
	if (result != kWorldOkay)
	{
		return result;
	}
	
	camera.Preprocess();
	SetCamera(&camera);

	// Play main game music
	music = new Sound;
	WaveStreamer *streamer = new WaveStreamer;
	//branch here for differing music by world
	streamer->AddComponent("A_Light_Groove");
	music->Stream(streamer);
	music->SetLoopCount(kSoundLoopInfinite);
	music->Play();


	return kWorldOkay;
	
}

void GameWorld::Interact(void)
{
	//The interact function is called once per frame. Before calling the base
	//class's Interact() function, set up the interaction probe to be in a line 
	//segment extending two meters from the players head in the direction that the 
	//camera is looking

	/*
	HandController *controller = TheGame->GetHandController();
	if (controller)
	{
		const Point3D& p = controller->GetTargetNode()->GetWorldPosition();
		Point3D position(p.x, p.y, p.z + 1.5F);

		//const Vector3D& direction = chaseCamera.GetWorldTransform()[2];
		controller->GetHandInteractor()->SetInteractionProbe(position, position * 1.0F);
	}
	*/

	World::Interact();
}

void GameWorld::Render(void)
{
	World::Render();
}