#pragma once

#include "C4Application.h"
#include "C4Engine.h"
#include "C4World.h"
#include "C4Interface.h"
#include "C4Methods.h"
#include "C4Files.h"

#include "GameWorld.h"
#include "ResetAction.h"
#include "HandController.h"
#include "Player.h"
#include "LightPathController.h"
#include "LightParticleSystem.h"
#include "ScriptMethods.h"
#include "Ring.h"
#include "NameChangeRequestMessage.h"

#define NUM_BEST_TIMES 6
#define TIME_FILE_PATH "Save/times"
#define SETTING_FILE_PATH "Save/settings"

using namespace C4;

//const unsigned long kGameProtocol = 0x00000012;
//const unsigned short kGamePort = 3003;

enum
{
	kLocatorAnimatedObject = 'aobj'
};

enum LevelEndState
{
	levelEndNone = 'none',
	levelEndComplete = 'comp',
	levelEndFailed = 'fail'
};

enum Level
{
	levelMenu = 'menu',
	levelOne = 'lone',
	levelTwo = 'ltwo',
	levelThree = 'ltre',
	levelFour = 'lfor',
	levelFive = 'lfiv',
	levelSix = 'lsix'
};

enum
{
	kModelDowner = 'down',
	kModelSpeedBoost = 'sbst',
	kModelRingExpander = 'rexp',
	kModelHandHeldSpeedBoost = 'hhsb',
	kModelHandHeldRingExpander = 'hhrx',
	kModelSmallBuilding = 'sbil',
	kModelLargeBuilding = 'lbil',
	
	kGameProtocol = 0x0000000B,
	kGamePort = 28327
};

class Game : public Singleton<Game>, public Application
{
private:

	DisplayEventHandler						displayEventHandler;
	ResetAction*							resetAction;

	ModelRegistration						playerModelReg;
	ModelRegistration						gauntletModelReg;
	ModelRegistration						ringSmallModelReg;
	ModelRegistration						ringMediumModelReg;
	ModelRegistration						ringLargeModelReg;
	ModelRegistration						downerModelReg;
	ModelRegistration						speedBoostModelReg;
	ModelRegistration						ringExpanderModelReg;
	ModelRegistration						handHeldSpeedBoostModelReg;
	ModelRegistration						handHeldRingExpanderModelReg;
	ModelRegistration						smallBuildingModelReg;
	ModelRegistration						largeBuildingModelReg;

	ControllerReg<HandController>			handControllerReg;
	ControllerReg<MenuHandController>		menuHandControllerReg;
	ControllerReg<MainPlayerController>		playerControllerReg;
	ControllerReg<LightPathController>		lightPathControllerReg;
	ControllerReg<RingController>			ringControllerReg;

	LocatorRegistration						animatedHand;

	ParticleSystemReg<LightParticleSystem>	lightParticleSystemReg;

	MethodReg<QuitMethod>					quitMethodReg;
	MethodReg<LoadWorldMethod>				loadWorldMethodReg;
	MethodReg<GetLevelResultMethod>			getLevelResultMethodReg;
	MethodReg<GetTimeStringMethod>			getTimeStringMethodReg;
	MethodReg<GetBestTimeStringMethod>		getBestTimeStringMethodReg;
	MethodReg<ClearScoresMethod>			clearScoresMethodReg;
	MethodReg<SetGameSettingsMethod>		setGameSettingsMethodReg;
	MethodReg<GetGameSettingsMethod>		getGameSettingsMethodReg;
	MethodReg<AddSpeedMethod>				addSpeedMethodReg;
	MethodReg<HostMethod>					hostMethodReg;
	MethodReg<JoinMethod>					joinMethodReg;

	HandController							*handController;

	static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);

	String<128>	loadLevel;
	LevelEndState levelEndState;
	Level lastLevel;
	int lastLevelTime;
	int bestTimes[NUM_BEST_TIMES];
	int settings[4];

	MainPlayerController* playerController;

	Sound* music;

	//Console Commands and netrowking stuff
	CommandObserver<Game>	serverObserver;
	CommandObserver<Game>	joinObserver;
	CommandObserver<Game>	nameObserver;
	//this will start a new server
	Command serverCommand;
	//this will join an existing game
	Command joinCommand;
	//The name change command will attempt to change the players name
	Command nameCommand;

public:

	Quaternion lookOrigin;

	Game(void);
	~Game(void);

	static World* ConstructWorld(const char* name, void* cookie);

	void StartLevel(const char* name);
	static void LoadLevel(DeferredTask* task, void* cookie);

	HandController *GetHandController(void) const
	{
		return (handController);
	}
	
	EngineResult LoadWorld(const char *name) override;
	void UnloadWorld(void) override;

	void SetLevelEndState(LevelEndState levelEndState);
	LevelEndState GetLevelEndState(void);
	Level GetLastLevel(void);
	void SetLastLevelTime(int lastLevelTime);
	String<> GetTimeString(void);
	String<> GetBestTimeString(Level level);
	void ClearScores(void);
	int GetMusicVolume(void);
	int GetSoundVolume(void);
	int GetTurnSensitivity(void);
	int GetRiftSensitivity(void);
	void SetMusicVolume(int musicVolume);
	void SetSoundVolume(int soundVolume);
	void SetTurnSensitivity(int turnSensitivity);
	void SetRiftSensitivity(int riftSensitivity);
	void SaveSettings(void);
	void SetPlayerController(MainPlayerController* playerController);
	MainPlayerController* GetPlayerController(void);

	//networking stuff
	void HostGame();
	void JoinGame(String<> ipAddress);
	// This method will be executed whenever the user uses the server command
	void ServerCommand(Command *command, const char *params);
	// This method will be executed whenever the user uses the join command.
	void JoinCommand(Command *command, const char *params);
	//This method will be executed whenevere the user uses the name command.
	void NameCommand(Command *command, const char *params);

	//this method will be called by the enegine whenever a chat is recieved
	void HandlePlayerEvent(PlayerEvent event, Player *player, const void *param);

	//this is called whenever the local sysytem recieves a Message from a specific player
	//On the server system, this method will respond to both locally sent messages (a system can send
	//messages to itself) or from one of the connect clients. For clien systems, this method will 
	//almost always respond to messages from the server - a client cannot normally connect to other clients
	void ReceiveMessage(Player *from, const NetworkAddress &address, const Message *message);

	//This method will be called whenever the messagemanager recieves a message.
	//Its task is to create an instance of a Message based on the Message Type given to it.
	Message *ConstructMessage(MessageType type, Decompressor &data) const;
};

extern "C"
{
	C4MODULEEXPORT C4::Application* ConstructApplication(void);
}

extern Game *TheGame;
