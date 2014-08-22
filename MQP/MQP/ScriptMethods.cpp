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