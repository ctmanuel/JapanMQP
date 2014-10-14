#include "Game.h"
#include "HandController.h"
#include "Multiplayer.h"


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

	//Environemnt Models
	smallBuildingModelReg(kModelSmallBuilding, "Small Building", "Model/smallBuilding"),
	largeBuildingModelReg(kModelLargeBuilding, "Large Building", "Model/largeBuilding"),
	CityWall_1(kModelCityWall, "Green City Wall 1", "Model/Environment/CityWall1"),
	CityWall_2(kModelCityWall2, "Green City Wall 2", "Model/Environment/CityWall2"),
	CityWall_3(kModelCityWall3, "Green City Wall 3", "Model/Environment/CityWall3"),
	BlueCityWall_1(kModelBlueCityWall, "Blue City Wall 1", "Model/Environment/BlueCityWall1"),
	BlueCityWall_2(kModelBlueCityWall2, "Blue City Wall 2", "Model/Environment/BlueCityWall2"),
	BlueCityWall_3(kModelBlueCityWall3, "Blue City Wall 3", "Model/Environment/BlueCityWall3"),
	CityPillar_1(kModelCityPillar, "Green City Pillar 1", "Model/Environment/CityPillar1"),
	CityPillar_2(kModelCityPillar2, "Green City Pillar 2", "Model/Environment/CityPillar2"),
	CityPillar_3(kModelCityPillar3, "Green City Pillar 3", "Model/Environment/CityPillar3"),
	BlueCityPillar_1(kModelBlueCityPillar, "Blue City Pillar 1", "Model/Environment/BlueCityPillar1"),
	BlueCityPillar_2(kModelBlueCityPillar2, "Blue City Pillar 2", "Model/Environment/BlueCityPillar2"),
	BlueCityPillar_3(kModelBlueCityPillar3, "Blue City Pillar 3", "Model/Environment/BlueCityPillar3"),
	CityCube_1(kModelCityCube, "Green City Cube 1", "Model/Environment/CityCube1"),
	CityCube_2(kModelCityCube2, "Green City Cube 2", "Model/Environment/CityCube2"),
	CityCube_3(kModelCityCube3, "Green City Cube 3", "Model/Environment/CityCube3"),
	BlueCityCube_1(kModelBlueCityCube, "Blue City Cube 1", "Model/Environment/BlueCityCube1"),
	BlueCityCube_2(kModelBlueCityCube2, "Blue City Cube 2", "Model/Environment/BlueCityCube2"),
	BlueCityCube_3(kModelBlueCityCube3, "Blue City Cube 3", "Model/Environment/BlueCityCube3"),

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
		settings[2] = 20;
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
		String<128> str("Loading World ");
		str += name;
		TheEngine->Report(str, kReportError);
		if (TheMessageMgr->Server())
		{
			TheMessageMgr->BeginMultiplayerGame(true);
		}
		else{
			TheMessageMgr->BeginSinglePlayerGame();
		}
		
	}

	return (result);
}

void Game::UnloadWorld(void)
{
	music->Stop();
	music->Release();
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

void Game::SpawnPlayer(Player *player, Point3D location, int32 controllerIndex)
{
	World *world = TheWorldMgr->GetWorld();
	if (world)
	{
		Point3D point(2.0f, 0.0f, 2.0f);

		GamePlayer *gPlayer = static_cast<GamePlayer *>(player);
		MainPlayerController *cont = new MainPlayerController();
		HandController *hcont = new HandController();

		if (!cont)
		{
			cont->SetControllerIndex(controllerIndex);
			hcont->SetControllerIndex(controllerIndex *= 2);

			gPlayer->SetController(cont);

			Model *player = Model::Get(kModelPlayer);
			Model *hand = Model::Get(kModelAnimatedHand);
			player->SetController(cont);
			hand->SetController(hcont);

			player->SetNodePosition(location);
			hand->SetNodePosition(point);

			world->AddNewNode(player);
			world->AddNewNode(hand);
			Engine::Report("Spawning player");
		}
		//Model *playereee = Model::Get(kModelRingExpander);

		//playereee->SetNodePosition(location);
		//world->AddNewNode(playereee);
		//Engine::Report("Spawning ring");
		
	}
}

void Game::HostGame()
{
	TheEngine->Report("Hosting Game");
	TheMessageMgr->BeginMultiplayerGame(true);
	TheEngine->Report(String<>("Initialized. Hosting on: ") + MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress(), true));

	//TheGame->LoadWorld("test");
	StartLevel("test");

	/*loadLevel = "test";
	DeferredTask* task = new DeferredTask(&LoadLevel, &loadLevel);
	task->SetTaskFlags(kTaskNonpersistent);
	TheTimeMgr->AddTask(task);*/
	
	TheMessageMgr->SendMessageAll(RequestMessage());
}

void Game::JoinGame(String<> ipAddress)
{

	TheMessageMgr->BeginMultiplayerGame(false);
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

void Game::JoinGame()
{
	Engine::Report("Sending broadcast query");
	//TheMessageMgr->BroadcastServerQuery();

}

void Game::ServerCommand(Command *command, const char *params)
{
	// Just start the server. The 'true' parameter 
	// is to indicate that this machine is the server.
	TheMessageMgr->BeginMultiplayerGame(true);
	TheMessageMgr->BroadcastServerQuery();
	
	//CANT USE MULTITHREADING CUZ THINGS GET SPAWNED IN THE MENU FUUUUUUCCCCCCCCCCCC
	TheGame->LoadWorld("test");

	/*loadLevel = "test";
	DeferredTask* task = new DeferredTask(&LoadLevel, &loadLevel);
	task->SetTaskFlags(kTaskNonpersistent);
	TheTimeMgr->AddTask(task);*/

	TheEngine->Report("Server initialized", kReportError);
	TheMessageMgr->SendMessageAll(RequestMessage());
}

void Game::JoinCommand(Command *command, const char *params)
{
	//temp
	TheNetworkMgr->SetPortNumber(0);
	String<128> str("Attempting to join ");
	str += params;
	TheEngine->Report(str, kReportError);

	TheMessageMgr->BeginMultiplayerGame(false);
	NetworkAddress address = MessageMgr::StringToAddress(params);
	address.SetPort(kGamePort);
	TheMessageMgr->Connect(address);
}

void Game::HandlePlayerEvent(PlayerEvent event, Player *player, const void *param)
{
	switch (event)
	{
		// We've received a chat. 
		case kPlayerChatReceived:
		{
				String<kMaxChatMessageLength + kMaxPlayerNameLength + 2> text(player->GetPlayerName());
				text += ": ";
				text += static_cast<const char *>(param);

				TheEngine->Report(text, kReportError);
				break;
		}

		case kPlayerInitialized:
		{
				Engine::Report("Player initialized");
				GamePlayer *gp = nullptr;
				MainPlayerController *controller = nullptr;
				Node *node = nullptr;
				PlayerKey key = -1;
				long id = -1;
				Point3D loc;

				Player *p = TheMessageMgr->GetFirstPlayer();
				while (p)
				{
					/*gp = static_cast<GamePlayer *>(p);
					
					controller = gp->GetController();
					if (controller)
					{
						node = controller->GetTargetNode();

						key = gp->GetPlayerKey();
						id = controller->GetControllerIndex();
						loc = node->GetWorldPosition();

						TheMessageMgr->SendMessage(player->GetPlayerKey(), SpawnMessage(key, id, loc));
					}*/
					Point3D loc2;
					loc2.x = 2.0f;
					loc2.y = 1.0f;
					loc2.z = 1.5f;

					TheMessageMgr->SendMessage(player->GetPlayerKey(), SpawnMessage(key, id, loc2));

					p = p->Next();
				}
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

//When the message manager recieves a message 
Message * Game::ConstructMessage(MessageType type, Decompressor &data) const
{
	Engine::Report(String<>("ConstructMessage: Got type ") + type);
	switch (type)
	{
		case kMessageSpawn:
			return (new SpawnMessage);
		case (kMessageNameChangeRequestMessage) :
			return (new NameChangeRequestMessage);

		case (kMessageNameChangeMessage) :
			return (new NameChangeMessage);
		
		case(kConnectionQueryReceived) :
		{
			Engine::Report("Constructing server info message");
			return new ServerInfoMessage;
		}
		case kMessageRequest:
		{
			Engine::Report("Constructing message request message");
			return new RequestMessage;
		}

		case kMessageBattleLevel:
		{
			Engine::Report("Constructing message load multiplayer message");
			return new LoadMultiplayerLevel;
		}
		default:
		{
			Engine::Report(String<>("Could not determine type: ") + type);
			return nullptr;
		}
	}
}

void Game::ReceiveMessage(Player *from, const NetworkAddress &address, const Message *message)
{
	MessageType type = message->GetMessageType();
	Engine::Report(String<>("ReceiveMessage: Got Message Type ") + type);
	
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

		case kMessageBattleLevel:
		{
			Engine::Report("Loading level");
		}
		case C4::kMessageServerInfo:
		{	
			Engine::Report("Recieved Message Server Info");
			String<> addr(MessageMgr::AddressToString(address, true));
			JoinGame(addr);
			break;
		}
		case kMessageRequest:
		{
								if (TheMessageMgr->Server())
								{
									Engine::Report("Tis the server");
									Point3D loc;
									loc.x = 3.0f;
									loc.y = 0.0f;
									loc.z = 2.0f;

									Point3D loc2;
									loc2.x = 0.0f;
									loc2.y = 0.0f;
									loc2.z = 0.0f;

									long contIndex = TheWorldMgr->GetWorld()->NewControllerIndex();
									TheMessageMgr->SendMessageAll(SpawnMessage(from->GetPlayerKey(), contIndex, loc));
								}
		}
	}
}

void Game::HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param)
{
	Engine::Report(String<> ("Recieved Connection event ") + event);
	
	switch (event)
	{
		case kConnectionQueryReceived:
		{
			Engine::Report("Recieved kConnectionQueryReceived");
			//send connectionless package to client that requested query
			ServerInfoMessage msg(2, 2, "New Game");
			TheMessageMgr->SendConnectionlessMessage(address, msg);

			break;
		}
		case kConnectionClientOpened:
		{
			Engine::Report("Client Connected");
			//TheMessageMgr->SendMessageAll(LoadMultiplayerLevel(23));
			break;
		}
		case kConnectionClientClosed:
		{
										Engine::Report("Client Connection Closed");
										break;
		}
		case kConnectionClientTimedOut:
		{
			Engine::Report("Client Timed Out");
			break;
		}
		case kConnectionServerAccepted:
		{
			Engine::Report("We are connected");
			//load world here?
			//send messageall request here?
			break;
		}
		case C4::kConnectionServerClosed:
		case C4::kConnectionServerTimedOut:
		{
			Engine::Report("Server Connection Closed.");
			//UnloadWorld();
			break;
		}
		case C4::kConnectionAttemptFailed:
		{
			Engine::Report("Connection Attemp Failed.");
			break;
		}
	}

	Application::HandleConnectionEvent(event, address, param);
}

