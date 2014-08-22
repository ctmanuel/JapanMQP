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

#define NUM_BEST_TIMES 6
#define TIME_FILE_PATH "Save/times"
#define SETTING_FILE_PATH "Save/settings"

using namespace C4;

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
	levelTwo = 'ltwo'
};

class Game : public Singleton<Game>, public Application
{
private:

	DisplayEventHandler						displayEventHandler;
	ResetAction*							resetAction;

	ModelRegistration						playerModelReg;
	ModelRegistration						gauntletModelReg;

	ControllerReg<HandController>			handControllerReg;
	ControllerReg<MenuHandController>		menuHandControllerReg;
	ControllerReg<MainPlayerController>		playerControllerReg;
	ControllerReg<LightPathController>		lightPathControllerReg;

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

	HandController							*handController;

	static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);

	String<128>	loadLevel;
	LevelEndState levelEndState;
	Level lastLevel;
	int lastLevelTime;
	int bestTimes[NUM_BEST_TIMES];
	int settings[4];

public:

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
};

extern "C"
{
	C4MODULEEXPORT Application* ConstructApplication(void);
}

extern Game *TheGame;
