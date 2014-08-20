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
	TheGame->StartLevel();
	CallCompletionProc();
}