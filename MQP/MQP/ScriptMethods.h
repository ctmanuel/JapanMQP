#pragma once

#include "C4Methods.h"
#include "C4World.h"

namespace C4
{

	enum
	{
		kMethodQuit = 'quit',
		kMethodLoadWorld = 'load',
		kMethodGetLevelResult = 'levl'
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

	class LoadWorldMethod : public Method
	{
	private:

		String<128> name;

		LoadWorldMethod(const LoadWorldMethod& loadWorldMethod);
		Method* Replicate(void) const;

	public:

		LoadWorldMethod();
		LoadWorldMethod(const char* name);
		~LoadWorldMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};

	class GetLevelResultMethod : public Method
	{
	private:

		GetLevelResultMethod(const GetLevelResultMethod& getLevelResultMethod);
		Method* Replicate(void) const;

	public:

		GetLevelResultMethod();
		~GetLevelResultMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};
}
