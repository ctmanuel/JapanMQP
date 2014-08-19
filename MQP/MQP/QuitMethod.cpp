#include "QuitMethod.h"

using namespace C4;

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