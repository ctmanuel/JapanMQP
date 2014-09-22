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

	//Networking stuff
	serverObserver(this, &Game::ServerCommand),
	joinObserver(this, &Game::JoinCommand),
	nameObserver(this, &Game::NameCommand),
	//Create the server command to bind the text 'server' in the console.
	serverCommand("server", &serverObserver),
	//Create the join command to bind to the text 'join' in the console.
	joinCommand("join", &joinObserver),
	nameCommand("name", &nameObserver),

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

	//Register both console commands with the engine
	TheEngine->AddCommand(&serverCommand);
	TheEngine->AddCommand(&joinCommand);
	TheEngine->AddCommand(&nameCommand);

	//Set some settings in the network manager
	TheNetworkMgr->SetProtocol(kGameProtocol);
	TheNetworkMgr->SetPortNumber(kGamePort);

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

	
	TheEngine->Report("Hosting Game");
	TheMessageMgr->BeginMultiplayerGame(true);
	TheEngine->Report(String<>("Initialized. Hosting on: ") + MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress(), true));

	TheGame->LoadWorld("Menu");
	
	//TheMessageMgr->SendMessageAll(RequestMessage());
}

void Game::JoinGame(String<> ipAddress)
{

	TheMessageMgr->BeginMultiplayerGame(false);
	TheMessageMgr->BroadcastServerQuery();
	NetworkAddress addr = MessageMgr::StringToAddress(ipAddress);
	addr.SetPort(kGamePort);
	TheEngine->Report(String<>("Attempting connection with: ") + MessageMgr::AddressToString(addr, true));
	NetworkResult result = TheMessageMgr->Connect(addr);
	//TheMessageMgr->

	if (result == C4::kNetworkOkay)
	{
		TheEngine->Report("Network initialized. Waiting on response...");
	}
	else
	{
		TheEngine->Report(String<>("Issues arose when trying to initialize the connection. Code: ") += result);
	}
}

void Game::ServerCommand(Command *command, const char *params)
{
	// Just start the server. The 'true' parameter 
	// is to indicate that this machine is the server.
	TheMessageMgr->BeginMultiplayerGame(true);
	TheEngine->Report("Server initialized", kReportError);
}

void Game::JoinCommand(Command *command, const char *params)
{
	// We'll first want to provide the user with some feedback - so he'll know what he's doing.
	String<128> str("Attempting to join ");
	str += params;
	TheEngine->Report(str, kReportError);

	// Next, we convert the entered parameters into a C4 NetworkAddress.
	// This format is used internally. It has both an IP address and a port number.
	NetworkAddress address = MessageMgr::StringToAddress(params);

	// We explicitly set a port in this example - it defaults to 0.
	address.SetPort(kGamePort);

	// Now we're just going to (try to) connect to the entered address.
	TheMessageMgr->Connect(address);
}

void Game::HandlePlayerEvent(PlayerEvent event, Player *player, const void *param)
{
	switch (event)
	{
		// We've received a chat. 
	case kPlayerChatReceived:
	{
			// We'll want to display the player's name in front of the chat message,
			// so we'll first paste the player's name and his message together in a String object.
			// We limit the size of the displayed text using the String class, which automatically
			// cuts off text that exceeds the boundary set in the template parameter.

			String<kMaxChatMessageLength + kMaxPlayerNameLength + 2> text(player->GetPlayerName());
			text += ": ";
			text += static_cast<const char *>(param);

			// Next, we'll make the completed message appear in the console.
			// The kReportError parameter tells the engine to put the message in the console. 
			// It doesn't actually mean there's an error.

			TheEngine->Report(text, kReportError);
			break;
	}
	}

	// Finally, we pass the player event to the parent Application's HandlePlayerEvent method,
	// so it can display errors if needed. The method does nothing at the moment, but we'll
	// add it just in case it will somewhere in the future.
	Application::HandlePlayerEvent(event, player, param);
}

void Game::NameCommand(Command* command, const char *param)
{
	NameChangeRequestMessage message(param);
	TheMessageMgr->SendMessage(kPlayerServer, message);
}

Message * Game::ConstructMessage(MessageType type, Decompressor &data) const
{
	switch (type)
	{
		//Server
		case (kMessageNameChangeRequestMessage) :
			return (new NameChangeRequestMessage());

		//Client
		case (kMessageNameChangeMessage) :
			return new NameChangeMessage();
		
		//add cases for other types of messages

		default:
		{
		   Engine::Report("Error: Could not determine Message type");
		   return nullptr;
		}
	}
}

void Game::ReceiveMessage(Player *from, const NetworkAddress &address, const Message *message)
{
	MessageType type = message->GetMessageType();
	
	switch (type)
	{
		//server
		case (kMessageNameChangeRequestMessage):
		{
			//cast the generic message into a more defined NameChangeRequestMessage
			//so we can get the new name from it
			const NameChangeRequestMessage *msg = static_cast<const NameChangeRequestMessage *>(message);

			//find the player who sent the message
			Player *player = TheMessageMgr->GetPlayer(from->GetPlayerKey());
												   
			//TODO: Insert validation here, like checking for duplicate names, invalid player names, etc.

			//send message to all connected cilents informing them that a players name has changes.
			// this message is sent to local client as well
			NameChangeMessage ncmsg(NameChangeMessage(msg->GetNewName(), player->GetPlayerKey()));
			TheMessageMgr->SendMessageAll(ncmsg);

			break;
		}

		//client
		case(kMessageNameChangeMessage) :
		{
			//cast the message
			const NameChangeMessage *msg = static_cast<const NameChangeMessage *> (message);

			//Find the player
			Player *player = TheMessageMgr->GetPlayer(msg->GetPlayerKey());

			//change the name if the player exists
			if (player)
			{
				//write confirmation message now
				String<128> str("Player ");
				str += player->GetPlayerName();
				str += " changed name to ";
				str += msg->GetNewName();

				//Update the players name
				player->SetPlayerName(msg->GetNewName());

				//confirm to user
				TheEngine->Report(str, kReportError);
			}
			else{
				//player wasn't found
				String<128> str("Player ");
				str += msg->GetPlayerKey();
				str += " not found while trying to update name. =(";
				TheEngine->Report(str, kReportError);
			}
			break;
		}//case

		case kMessageServerInfo:
		{
			String<> addr(MessageMgr::AddressToString(address, true));
			JoinGame(addr);
			break;
		}
	}
}

/*void Game::HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param)
{
	switch (event)
	{
	case kConnectionQueryReceived:
		//do something
		break;
	}
}

*/