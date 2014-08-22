#include "ScriptMethods.h"
#include "Game.h"

using namespace C4;

extern Game* TheGame;

// Quit method
QuitMethod::QuitMethod() : Method(kMethodQuit)
{
}

QuitMethod::QuitMethod(const QuitMethod& quitMethod) : Method(quitMethod)
{
}

QuitMethod::~QuitMethod()
{
}

Method* QuitMethod::Replicate(void) const
{
	return (new QuitMethod(*this));
}

void QuitMethod::Pack(Packer& data, unsigned long packFlags) const
{
	Method::Pack(data, packFlags);
}

void QuitMethod::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Method::Unpack(data, unpackFlags);
}

int32 QuitMethod::GetSettingCount(void) const
{
	return 0;
}

Setting* QuitMethod::GetSetting(long index) const
{
	return nullptr;
}

void QuitMethod::SetSetting(const Setting* setting)
{
}

void QuitMethod::Execute(const ScriptState* state)
{
	TheEngine->Quit();
}


// Load World method
LoadWorldMethod::LoadWorldMethod() : Method(kMethodLoadWorld)
{
	name = "";
}

LoadWorldMethod::LoadWorldMethod(const char* name) : Method(kMethodLoadWorld)
{
	this->name = name;
}

LoadWorldMethod::~LoadWorldMethod()
{
}

LoadWorldMethod::LoadWorldMethod(const LoadWorldMethod& loadWorldMethod) : Method(loadWorldMethod)
{
	name = loadWorldMethod.name;
}

Method* LoadWorldMethod::Replicate(void) const
{
	return (new LoadWorldMethod(*this));
}

void LoadWorldMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << name;
}

void LoadWorldMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> name;
}

int32 LoadWorldMethod::GetSettingCount(void) const
{
	return (1);
}

Setting* LoadWorldMethod::GetSetting(int32 index) const
{
	if (index == 0)
	{
		return (new TextSetting('wrld', name, "World", 256));
	}

	return nullptr;
}

void LoadWorldMethod::SetSetting(const Setting* setting)
{
	if (setting->GetSettingIdentifier() == 'wrld')
	{
		name = ((TextSetting*)setting)->GetText();
	}
}

void LoadWorldMethod::Execute(const ScriptState* state)
{
	if (!Text::CompareText(name, "Menu"))
	{
		TheGame->SetLevelEndState(levelEndComplete);
	}
	TheGame->StartLevel(name);
	CallCompletionProc();
}


// Get Level Result method
GetLevelResultMethod::GetLevelResultMethod() : Method(kMethodGetLevelResult)
{
}

GetLevelResultMethod::~GetLevelResultMethod()
{
}

GetLevelResultMethod::GetLevelResultMethod(const GetLevelResultMethod& getLevelResultMethod) : Method(getLevelResultMethod)
{
}

Method* GetLevelResultMethod::Replicate(void) const
{
	return (new GetLevelResultMethod(*this));
}

void GetLevelResultMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);
}

void GetLevelResultMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
}

int32 GetLevelResultMethod::GetSettingCount(void) const
{
	return (0);
}

Setting* GetLevelResultMethod::GetSetting(int32 index) const
{
	return nullptr;
}

void GetLevelResultMethod::SetSetting(const Setting* setting)
{
}

void GetLevelResultMethod::Execute(const ScriptState* state)
{
	Vector3D result = Vector3D(0.0f, 0.0f, 0.0f);

	switch (TheGame->GetLevelEndState())
	{
	case levelEndNone:
		result.x = 0.0f;
		break;
	case levelEndComplete:
		result.x = 1.0f;
		break;
	case levelEndFailed:
		result.x = 2.0f;
		break;
	}

	switch (TheGame->GetLastLevel())
	{
	case levelMenu:
		result.y = 0.0f;
		break;
	case levelOne:
		result.y = 1.0f;
		break;
	case levelTwo:
		result.y = 2.0f;
		break;
	}

	SetOutputValue(state, result);
	CallCompletionProc();
}


// Get Time String method
GetTimeStringMethod::GetTimeStringMethod() : Method(kMethodGetTimeString)
{
}

GetTimeStringMethod::~GetTimeStringMethod()
{
}

GetTimeStringMethod::GetTimeStringMethod(const GetTimeStringMethod& getTimeStringMethod) : Method(getTimeStringMethod)
{
}

Method* GetTimeStringMethod::Replicate(void) const
{
	return (new GetTimeStringMethod(*this));
}

void GetTimeStringMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);
}

void GetTimeStringMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
}

int32 GetTimeStringMethod::GetSettingCount(void) const
{
	return (0);
}

Setting* GetTimeStringMethod::GetSetting(int32 index) const
{
	return nullptr;
}

void GetTimeStringMethod::SetSetting(const Setting* setting)
{
}

void GetTimeStringMethod::Execute(const ScriptState* state)
{
	String<> s = TheGame->GetTimeString();

	SetOutputValue(state, s);
	CallCompletionProc();
}


// Get Best Time String method
GetBestTimeStringMethod::GetBestTimeStringMethod() : Method(kMethodGetBestTimeString)
{
	level = 0;
}

GetBestTimeStringMethod::~GetBestTimeStringMethod()
{
}

GetBestTimeStringMethod::GetBestTimeStringMethod(const GetBestTimeStringMethod& getBestTimeStringMethod) : Method(getBestTimeStringMethod)
{
	level = getBestTimeStringMethod.level;
}

Method* GetBestTimeStringMethod::Replicate(void) const
{
	return (new GetBestTimeStringMethod(*this));
}

void GetBestTimeStringMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << level;
}

void GetBestTimeStringMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> level;
}

int32 GetBestTimeStringMethod::GetSettingCount(void) const
{
	return (1);
}

Setting* GetBestTimeStringMethod::GetSetting(int32 index) const
{
	if (index == 0)
	{
		return (new IntegerSetting('levl', level, "Level", 0, 6, 1));
	}
	return nullptr;
}

void GetBestTimeStringMethod::SetSetting(const Setting* setting)
{
	if (setting->GetSettingIdentifier() == 'levl')
	{
		level = ((IntegerSetting*)setting)->GetIntegerValue();
	}
}

void GetBestTimeStringMethod::Execute(const ScriptState* state)
{
	String<> s = "Didn't get a string";
	switch (level)
	{
	case 0:
		s = "The menu doesn't have a best time, silly!";
		break;
	case 1:
		s = TheGame->GetBestTimeString(levelOne);
		break;
	case 2:
		s = TheGame->GetBestTimeString(levelTwo);
		break;
	}

	SetOutputValue(state, s);
	CallCompletionProc();
}


// Clear Scores method
ClearScoresMethod::ClearScoresMethod() : Method(kMethodClearScores)
{
}

ClearScoresMethod::ClearScoresMethod(const ClearScoresMethod& clearScoresMethod) : Method(clearScoresMethod)
{
}

ClearScoresMethod::~ClearScoresMethod()
{
}

Method* ClearScoresMethod::Replicate(void) const
{
	return (new ClearScoresMethod(*this));
}

void ClearScoresMethod::Pack(Packer& data, unsigned long packFlags) const
{
	Method::Pack(data, packFlags);
}

void ClearScoresMethod::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Method::Unpack(data, unpackFlags);
}

int32 ClearScoresMethod::GetSettingCount(void) const
{
	return 0;
}

Setting* ClearScoresMethod::GetSetting(long index) const
{
	return nullptr;
}

void ClearScoresMethod::SetSetting(const Setting* setting)
{
}

void ClearScoresMethod::Execute(const ScriptState* state)
{
	TheGame->ClearScores();
	CallCompletionProc();
}


// Set Game Settings method
SetGameSettingsMethod::SetGameSettingsMethod() : Method(kMethodSetGameSettings)
{
	musicVolume = 100;
	soundVolume = 100;
	turnSensitivity = 50;
	riftSensitivity = 50;
}

SetGameSettingsMethod::~SetGameSettingsMethod()
{
}

SetGameSettingsMethod::SetGameSettingsMethod(const SetGameSettingsMethod& setGameSettingsMethod) : Method(setGameSettingsMethod)
{
	musicVolume = setGameSettingsMethod.musicVolume;
	soundVolume = setGameSettingsMethod.soundVolume;
	turnSensitivity = setGameSettingsMethod.turnSensitivity;
	riftSensitivity = setGameSettingsMethod.riftSensitivity;
}

Method* SetGameSettingsMethod::Replicate(void) const
{
	return (new SetGameSettingsMethod(*this));
}

void SetGameSettingsMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << musicVolume;
	data << soundVolume;
	data << turnSensitivity;
	data << riftSensitivity;
}

void SetGameSettingsMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> musicVolume;
	data >> soundVolume;
	data >> turnSensitivity;
	data >> riftSensitivity;
}

int32 SetGameSettingsMethod::GetSettingCount(void) const
{
	return (4);
}

Setting* SetGameSettingsMethod::GetSetting(int32 index) const
{
	switch (index)
	{
	case 0:
		return (new IntegerSetting('musc', musicVolume, "Music Volume", 0, 100, 1));
		break;
	case 1:
		return (new IntegerSetting('sond', soundVolume, "Sound Volume", 0, 100, 1));
		break;
	case 2:
		return (new IntegerSetting('turn', turnSensitivity, "Turn Sensitivity", 0, 100, 1));
		break;
	case 3:
		return (new IntegerSetting('rift', riftSensitivity, "Rift Sensitivity", 0, 100, 1));
		break;
	}
	return nullptr;
}

void SetGameSettingsMethod::SetSetting(const Setting* setting)
{
	switch (setting->GetSettingIdentifier())
	{
	case 'musc':
		musicVolume = ((IntegerSetting*)setting)->GetIntegerValue();
		break;
	case 'sond':
		soundVolume = ((IntegerSetting*)setting)->GetIntegerValue();
		break;
	case 'turn':
		turnSensitivity = ((IntegerSetting*)setting)->GetIntegerValue();
		break;
	case 'rift':
		riftSensitivity = ((IntegerSetting*)setting)->GetIntegerValue();
		break;
	}
}

void SetGameSettingsMethod::Execute(const ScriptState* state)
{
	TheGame->SetMusicVolume(musicVolume);
	TheGame->SetSoundVolume(soundVolume);
	TheGame->SetTurnSensitivity(turnSensitivity);
	TheGame->SetRiftSensitivity(riftSensitivity);
	TheGame->SaveSettings();
	CallCompletionProc();
}


// Get Game Settings method
GetGameSettingsMethod::GetGameSettingsMethod() : Method(kMethodGetGameSettings)
{
}

GetGameSettingsMethod::~GetGameSettingsMethod()
{
}

GetGameSettingsMethod::GetGameSettingsMethod(const GetGameSettingsMethod& getGameSettingsMethod) : Method(getGameSettingsMethod)
{
}

Method* GetGameSettingsMethod::Replicate(void) const
{
	return (new GetGameSettingsMethod(*this));
}

void GetGameSettingsMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);
}

void GetGameSettingsMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
}

int32 GetGameSettingsMethod::GetSettingCount(void) const
{
	return (0);
}

Setting* GetGameSettingsMethod::GetSetting(int32 index) const
{
	return nullptr;
}

void GetGameSettingsMethod::SetSetting(const Setting* setting)
{
}

void GetGameSettingsMethod::Execute(const ScriptState* state)
{
	ColorRGBA settings;
	settings.red = TheGame->GetMusicVolume();
	settings.green = TheGame->GetSoundVolume();
	settings.blue = TheGame->GetTurnSensitivity();
	settings.alpha = TheGame->GetRiftSensitivity();

	SetOutputValue(state, settings);
	CallCompletionProc();
}