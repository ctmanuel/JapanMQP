#pragma once

#include "C4Methods.h"
#include "C4World.h"

namespace C4
{

	enum
	{
		kMethodQuit = 'quit',
		kMethodLoadWorld = 'load',
		kMethodGetLevelResult = 'levl',
		kMethodGetTimeString = 'time',
		kMethodGetBestTimeString = 'best',
		kMethodClearScores = 'cler',
		kMethodSetGameSettings = 'sset',
		kMethodGetGameSettings = 'gset',
		kMethodAddSpeed = 'aspd'
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

	class GetTimeStringMethod : public Method
	{
	private:

		GetTimeStringMethod(const GetTimeStringMethod& getTimeStringMethod);
		Method* Replicate(void) const;

	public:

		GetTimeStringMethod();
		~GetTimeStringMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};

	class GetBestTimeStringMethod : public Method
	{
	private:

		int level;

		GetBestTimeStringMethod(const GetBestTimeStringMethod& getBestTimeStringMethod);
		Method* Replicate(void) const;

	public:

		GetBestTimeStringMethod();
		~GetBestTimeStringMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};

	class ClearScoresMethod : public Method
	{
	private:

		ClearScoresMethod(const ClearScoresMethod& clearScoresMethod);
		Method* Replicate(void) const;

	public:

		ClearScoresMethod();
		~ClearScoresMethod();

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(long index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};

	class SetGameSettingsMethod : public Method
	{
	private:

		int musicVolume, soundVolume, turnSensitivity, riftSensitivity;

		SetGameSettingsMethod(const SetGameSettingsMethod& setGameSettingsMethod);
		Method* Replicate(void) const;

	public:

		SetGameSettingsMethod();
		~SetGameSettingsMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};

	class GetGameSettingsMethod : public Method
	{
	private:

		GetGameSettingsMethod(const GetGameSettingsMethod& getGameSettingsMethod);
		Method* Replicate(void) const;

	public:

		GetGameSettingsMethod();
		~GetGameSettingsMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};

	class AddSpeedMethod : public Method
	{
	private:

		float speedChange;

		AddSpeedMethod(const AddSpeedMethod& AddSpeedMethod);
		Method* Replicate(void) const;

	public:

		AddSpeedMethod();
		AddSpeedMethod(const char* name);
		~AddSpeedMethod();

		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 upackFlags);

		int32 GetSettingCount(void) const;
		Setting* GetSetting(int32 index) const;
		void SetSetting(const Setting* setting);

		void Execute(const ScriptState* state);
	};
}
