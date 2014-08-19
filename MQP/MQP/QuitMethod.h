#pragma once

#include "C4Methods.h"

namespace C4
{
	enum
	{
		kMethodQuit = 'quit'
	};

	class QuitMethod : public Method
	{
	private:

		QuitMethod(const QuitMethod& quitMethod);
		Method* Replicate(void) const;

	public:

		QuitMethod();
		~QuitMethod();

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(long index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};
}
