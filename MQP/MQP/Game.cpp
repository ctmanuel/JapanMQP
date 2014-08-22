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
	loadWorldMethodReg(kMethodLoadWorld, "Load World"),
	getLevelResultMethodReg(kMethodGetLevelResult, "Get Level Result", kMethodOutputValue),
	getTimeStringMethodReg(kMethodGetTimeString, "Get Time String", kMethodOutputValue),
	getBestTimeStringMethodReg(kMethodGetBestTimeString, "Get Best Time String", kMethodOutputValue)
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
	levelEndState = levelEndNone;
	lastLevel = levelMenu;
	lastLevelTime = -1;

	File file;

	if (file.Open(TIME_FILE_PATH) != kFileOpenFailed)
	{
		file.Read(bestTimes, sizeof(int) * NUM_BEST_TIMES);
		file.Close();
	}
	else // file couldn't be openned, probably because it isn't there
	{
		file.Open(TIME_FILE_PATH, kFileCreate);
		for (int i = 0; i < NUM_BEST_TIMES; i++)
		{
			bestTimes[i] = -1;
		}
		file.Write(bestTimes, sizeof(int) * NUM_BEST_TIMES);
		file.Close();
	}

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

	if (Text::CompareTextCaseless(name, "menu") && (lastLevelTime != -1))
	{
		// Going to the menu
		// Check if player just got best time on a level
		int i = -1;
		switch (lastLevel)
		{
		case levelMenu: // Game just started
			break; 
		case levelOne:
			i = 0;
			break;
		case levelTwo:
			i = 1;
			break;
		}
		if (i != -1)
		{
			if ((bestTimes[i] == -1) || (lastLevelTime < bestTimes[i]))
			{
				bestTimes[i] = lastLevelTime;
				File file;
				file.Open(TIME_FILE_PATH, kFileReadWrite);
				file.Write(bestTimes, sizeof(int) * NUM_BEST_TIMES);
				file.Close();
			}
		}
	}
	else if (Text::CompareTextCaseless(name, "gameworld_01"))
	{
		lastLevel = levelOne;
	}
	else
	{
		lastLevel = levelTwo;
	}

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

		TheMessageMgr->BeginSinglePlayerGame();

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
	//TheGame = nullptr;
}

void Game::SetLevelEndState(LevelEndState levelEndState)
{
	this->levelEndState = levelEndState;
	if (levelEndState == levelEndFailed)
	{
		lastLevelTime = -1;
	}
}

LevelEndState Game::GetLevelEndState(void)
{
	return levelEndState;
}

Level Game::GetLastLevel(void)
{
	return lastLevel;
}

void Game::SetLastLevelTime(int lastLevelTime)
{
	this->lastLevelTime = lastLevelTime;
}

String<> Game::GetTimeString(void)
{
	int minutes = floor(lastLevelTime / 60000);
	int seconds = floor((lastLevelTime % 60000) / 1000);
	int centiseconds = floor((lastLevelTime % 1000) / 10);
	String<> s = "Your time: ";
	if (minutes < 10)
	{
		s += "0";
	}
	s += minutes;
	s += ":";
	if (seconds < 10)
	{
		s += "0";
	}
	s += seconds;
	s += ":";
	if (centiseconds < 10)
	{
		s += "0";
	}
	s += centiseconds;

	return s;
}

String<> Game::GetBestTimeString(Level level)
{
	int bestTime;
	int i;

	switch (level)
	{
	case levelOne:
		i = 0;
		break;
	case levelTwo:
		i = 1;
		break;
	}

	bestTime = bestTimes[i];
	String<> s = "Best Time: ";

	if (bestTime == -1)
	{
		s += "--:--:--";
		return s;
	}
	else
	{
		int minutes = floor(bestTime / 60000);
		int seconds = floor((bestTime % 60000) / 1000);
		int centiseconds = floor((bestTime % 1000) / 10);
		if (minutes < 10)
		{
			s += "0";
		}
		s += minutes;
		s += ":";
		if (seconds < 10)
		{
			s += "0";
		}
		s += seconds;
		s += ":";
		if (centiseconds < 10)
		{
			s += "0";
		}
		s += centiseconds;

		return s;
	}
}