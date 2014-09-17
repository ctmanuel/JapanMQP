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
	ringControllerReg(kControllerRing, "Ring"),

	//Model Registrations
	//Player model registration, Hand Model Registration, 
	playerModelReg(kModelPlayer, "Player", "Model/player", kModelPrecache, kControllerPlayer),
	gauntletModelReg(kModelAnimatedHand, "AnimatedGauntlet", "Model/gauntletAnimated", kModelPrecache, kControllerAnimatedHand),
	ringSmallModelReg(kModelRingSmall, "Small Ring", "Model/ringSmall"),
	ringMediumModelReg(kModelRingMedium, "Medium Ring", "Model/ringMedium"),
	ringLargeModelReg(kModelRingLarge, "Large Ring", "Model/ringLarge"),
	downerModelReg(kModelDowner, "Downer", "Model/downer"),
	speedBoostModelReg(kModelSpeedBoost, "Speed Boost", "Model/speedBoost"),
	ringExpanderModelReg(kModelRingExpander, "Ring Expander", "Model/ringExpander"),
	handHeldSpeedBoostModelReg(kModelHandHeldSpeedBoost, "Hand Held Speed Boost", "Model/handHeldSpeedBoost"),
	handHeldRingExpanderModelReg(kModelHandHeldRingExpander, "Hand Held Ring Expander", "Model/handHeldRingExpander"),
	smallBuildingModelReg(kModelSmallBuilding, "Small Building", "Model/smallBuilding"),
	largeBuildingModelReg(kModelLargeBuilding, "Large Building", "Model/largeBuilding"),

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
	getBestTimeStringMethodReg(kMethodGetBestTimeString, "Get Best Time String", kMethodOutputValue),
	clearScoresMethodReg(kMethodClearScores, "Clear Scores"),
	setGameSettingsMethodReg(kMethodSetGameSettings, "Set Game Settings"),
	getGameSettingsMethodReg(kMethodGetGameSettings, "Get Game Settings", kMethodOutputValue),
	addSpeedMethodReg(kMethodAddSpeed, "Add Speed"),
	hostMethodReg(kMethodHost, "Host"),
	joinMethodReg(kMethodJoin, "Join")
{
	// This installs an event handler for display events. This is only
	// necessary if we need to perform some action in response to
	// display events for some reason.
	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
	TheInterfaceMgr->HideCursor();
	TheInputMgr->SetInputMode(kInputAllActive);
	resetAction = new ResetAction(kActionReset);
	TheInputMgr->AddAction(resetAction);

	loadLevel = "";

	LoadWorld("Menu");
	handController = nullptr;
	levelEndState = levelEndNone;
	lastLevel = levelMenu;
	lastLevelTime = -1;

	File file;

	// Load times
	if (file.Open(TIME_FILE_PATH) != kFileOpenFailed)
	{
		file.Read(bestTimes, sizeof(int)* NUM_BEST_TIMES);
		file.Close();
	}
	else // file couldn't be openned, probably because it isn't there
	{
		for (int i = 0; i < NUM_BEST_TIMES; i++)
		{
			bestTimes[i] = -1;
		}
		file.Open(TIME_FILE_PATH, kFileCreate);
		file.Write(bestTimes, sizeof(int)* NUM_BEST_TIMES);
		file.Close();
	}

	// Load settings
	if (file.Open(SETTING_FILE_PATH) != kFileOpenFailed)
	{
		file.Read(settings, sizeof(int)* 4);
		file.Close();
	}
	else
	{
		settings[0] = 100;
		settings[1] = 100;
		settings[2] = 50;
		settings[3] = 50;
		file.Open(SETTING_FILE_PATH, kFileCreate);
		file.Write(settings, sizeof(int)* 4);
		file.Close();
	}

	// Load music and play menu music
	WaveStreamer* menuStreamer = new WaveStreamer;
	menuStreamer->AddComponent("Hikarimichi");
	music = new Sound;
	music->Stream(menuStreamer);
	music->SetLoopCount(kSoundLoopInfinite);
	music->Delay(10);
	music->VaryVolume((float)settings[0] / 100.0f, 0);

	playerController = nullptr;

	lookOrigin = TheWorldMgr->GetTrackingOrientation();
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

	if (Text::CompareTextCaseless(name, "menu"))
	{
		// Going to the menu

		// Play music
		music->Stop();
		music->Release();
		WaveStreamer* menuStreamer = new WaveStreamer;
		menuStreamer->AddComponent("Hikarimichi");
		music = new Sound;
		music->Stream(menuStreamer);
		music->SetLoopCount(kSoundLoopInfinite);
		music->Delay(10);
		music->VaryVolume((float)settings[0] / 100.0f, 0);


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
		case levelThree:
			i = 2;
			break;
		case levelFour:
			i = 3;
			break;
		case levelFive:
			i = 4;
			break;
		case levelSix:
			i = 5;
			break;
		}
		if (i != -1)
		{
			if (((bestTimes[i] == -1) || (lastLevelTime < bestTimes[i])) && (lastLevelTime != -1))
			{
				bestTimes[i] = lastLevelTime;
				File file;
				file.Open(TIME_FILE_PATH, kFileReadWrite);
				file.Write(bestTimes, sizeof(int) * NUM_BEST_TIMES);
				file.Close();
			}
		}
	}
	else // not the menu
	{
		// play music
		music->Stop();
		music->Release();
		music = new Sound;
		WaveStreamer *levelStreamer = new WaveStreamer;
		levelStreamer->AddComponent("A_Light_Groove");
		music->Stream(levelStreamer);
		music->SetLoopCount(kSoundLoopInfinite);
		music->Delay(10);
		music->VaryVolume((float)settings[0] / 100.0f, 0);

		if (Text::CompareTextCaseless(name, "level1"))
		{
			lastLevel = levelOne;
		}
		else if (Text::CompareTextCaseless(name, "level2"))
		{
			lastLevel = levelTwo;
		}
		else if (Text::CompareTextCaseless(name, "level3"))
		{
			lastLevel = levelThree;
		}
		else if (Text::CompareTextCaseless(name, "level4"))
		{
			lastLevel = levelFour;
		}
		else if (Text::CompareTextCaseless(name, "level5"))
		{
			lastLevel = levelFive;
		}
		else
		{
			lastLevel = levelSix;
		}
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
	}

	return (result);
}

void Game::UnloadWorld(void)
{
	TheWorldMgr->UnloadWorld();
	TheGame->handController = nullptr;
	playerController = nullptr;
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
	case levelThree:
		i = 2;
		break;
	case levelFour:
		i = 3;
		break;
	case levelFive:
		i = 4;
		break;
	case levelSix:
		i = 5;
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

void Game::ClearScores(void)
{
	for (int i = 0; i < NUM_BEST_TIMES; i++)
	{
		bestTimes[i] = -1;
	}

	File file;
	file.Open(TIME_FILE_PATH, kFileReadWrite);
	file.Write(bestTimes, sizeof(int)* NUM_BEST_TIMES);
	file.Close();
}

int Game::GetMusicVolume(void)
{
	return settings[0];
}

int Game::GetSoundVolume(void)
{
	return settings[1];
}

int Game::GetTurnSensitivity(void)
{
	return settings[2];
}

int Game::GetRiftSensitivity(void)
{
	return settings[3];
}

void Game::SetMusicVolume(int musicVolume)
{
	settings[0] = musicVolume;
	music->VaryVolume((float)settings[0] / 100.0f, 0);
}

void Game::SetSoundVolume(int soundVolume)
{
	settings[1] = soundVolume;
}

void Game::SetTurnSensitivity(int turnSensitivity)
{
	settings[2] = turnSensitivity;
}

void Game::SetRiftSensitivity(int riftSensitivity)
{
	settings[3] = riftSensitivity;
}

void Game::SaveSettings(void)
{
	File file;
	if (file.Open(SETTING_FILE_PATH, kFileReadWrite) != kFileOpenFailed)
	{
		file.Write(settings, sizeof(int) * 4);
		file.Close();
	}
}

void Game::SetPlayerController(MainPlayerController* playerController)
{
	this->playerController = playerController;
}

MainPlayerController* Game::GetPlayerController(void)
{
	return playerController;
}

void Game::HostGame()
{

	// temp
	TheEngine->Report("Poop");

	TheMessageMgr->BeginMultiplayerGame(true);
	TheEngine->Report(String<>("Initialized. Hosting on: ") + MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress(), true));

	//TheGame->LoadWorld("world/mult");

	//boxSnapShot = new BoxSnapShot();
	//TheMessageMgr->AddSnapshotSender(boxSnapShot);
}

void Game::JoinGame(String<> ipAddress)
{

	// temp
	TheEngine->Report("Poop");

	TheMessageMgr->BeginMultiplayerGame(false);
	NetworkAddress addr = MessageMgr::StringToAddress(ipAddress);
	addr.SetPort(kGamePort);
	NetworkResult result = TheMessageMgr->Connect(addr);
	TheEngine->Report(String<>("Attempting connection with: ") + MessageMgr::AddressToString(addr, true));

	if (result == C4::kNetworkOkay)
	{
		TheEngine->Report("Network initialized. Waiting on response...");
	}
	else
	{
		TheEngine->Report(String<>("Issues arose when trying to initialize the connection. Code: ") += result);
	}
}