#include "Game.h"
#include "HandController.h"

using namespace C4;

Game* TheGame = nullptr;

Application* ConstructApplication(void)
{
	return (new Game);
}

Game::Game() :

	Singleton<Game>(TheGame),														

	// The display event handler encapsulates a function that gets called
	// when the Display Manager changes something like the screen resolution.

	displayEventHandler(&HandleDisplayEvent),

	//Controller Registrations
	//Hand Controller registration, player controller registration, lightpath controller registration
	handControllerReg(kControllerAnimatedHand, "Hand"),
	menuHandControllerReg(kControllerMenuHand, "MenuHand"),
	playerControllerReg(kControllerPlayer, "Main Player Controller"),
	lightPathControllerReg(kControllerLightPath, "Light Path"),

	//Model Registrations
	//Player model registration, Hand Model Registration, 
	playerModelReg(kModelPlayer, "Player", "Model/player", kModelPrecache, kControllerPlayer),
	gauntletModelReg(kModelAnimatedHand, "AnimatedGauntlet", "Model/gauntletAnimated", kModelPrecache, kControllerAnimatedHand),
	
	//Animated Object Registration
	//Animated Hand Registration
	animatedHand(kLocatorAnimatedObject, "AnimatedGauntlet"),

	//Particle System Registration
	lightParticleSystemReg(kParticleSystemLight, "Light"),

	//Script Method Registration
	quitMethodReg(kMethodQuit, "Quit Game"),
	loadWorldMethodReg(kMethodLoadWorld, "Load World")
{
	// This installs an event handler for display events. This is only
	// necessary if we need to perform some action in response to
	// display events for some reason.
	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
	resetAction = new ResetAction(kActionReset);
	TheInputMgr->AddAction(resetAction);

	loadLevel = "";

	LoadWorld("Menu");
	handController = nullptr;
}

Game::~Game()
{
	TheWorldMgr->UnloadWorld();
	TheWorldMgr->SetWorldConstructor(nullptr);
	delete resetAction;
}

void Game::HandleDisplayEvent(const DisplayEventData *eventData, void *cookie)
{
	// This function is called when a display event occurs (because we
	// registered it in the Game constructor).

	if (eventData->eventType == kEventDisplayChange)
	{
		// The screen resolution has changed. Handle accordingly.
	}
}

World* Game::ConstructWorld(const char* name, void* cookie)
{
	return (new GameWorld(name));
}

void Game::StartLevel(const char* name)
{
	loadLevel = name;
	DeferredTask* task = new DeferredTask(&LoadLevel, &loadLevel);
	task->SetTaskFlags(kTaskNonpersistent);
	TheTimeMgr->AddTask(task);
}

void Game::LoadLevel(DeferredTask* task, void* cookie)
{
	TheWorldMgr->LoadWorld(*((String<128>*)cookie));
}

EngineResult Game::LoadWorld(const char *name)
{
	// Attempt to load the world.

	WorldResult result = TheWorldMgr->LoadWorld(name);
	if (result == kWorldOkay)
	{
		/*
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		Model *model = Model::Get(kModelAnimatedHand);
		HandController *controller = new HandController();
		model->SetController(controller);
		TheGame->handController = controller;
		*/
		//const LocatorMarker *locator = world->GetSpawnLocator();
		/*if (locator)
		{
			// If a spawn locator was found in the world, put a soldier character there.

			// The BeginSinglePlayerGame() function puts the Message Manager in single player mode.

			TheMessageMgr->BeginSinglePlayerGame();

			// Calculate the angle corresponding to the direction the character is initially facing.

			const Vector3D direction = locator->GetWorldTransform()[0];
			float azimuth = Atan(direction.y, direction.x);

			// Load a soldier model and attach a controller to it.

			Model *model = Model::Get(kModelSoldier);
			SoldierController *controller = new SoldierController(azimuth);
			model->SetController(controller);
			TheGame->soldierController = controller;

			// Put the model in the world at the locator's position.

			model->SetNodePosition(locator->GetWorldPosition());
			locator->GetWorld()->AddNewNode(model);

			// Set the world's current camera to be our chase camera.
			// The world will not render without a camera being set.

			ChaseCamera *camera = world->GetChaseCamera();
			camera->SetTargetModel(model);
			world->SetCamera(camera);
		}*/
	}

	return (result);
}

void Game::UnloadWorld(void)
{
	TheWorldMgr->UnloadWorld();
	TheGame->handController = nullptr;
	TheGame = nullptr;
}

