#pragma once

#include "C4Application.h"
#include "C4Messages.h"
#include "HandController.h"
#include "Player.h"
#include "Game.h"

using namespace C4;

enum MESSAGES
{
	kMessageServerInfo = kMessageBaseCount,
	kMessageRequest = 159,
	kMessageSpawn,
	kMessageOrientation,
	kMessageBattleLevel
};
namespace C4{

	class GamePlayer : public Player
	{
		private:
			MainPlayerController *cont;
			HandController *hcont;

		public:

			GamePlayer(PlayerKey key);
			~GamePlayer();

			MainPlayerController *GetController() const
			{
				return cont;
			}

			void SetController(MainPlayerController *controller)
			{
				cont = controller;
			}

			HandController *GetHandController() const
			{
				return hcont;
			}

			void SetHandController(HandController *controller)
			{
				hcont = controller;
			}
	};

	class ServerInfoMessage : public Message
	{
		friend class Game;

	private:

		int32	playerCount;
		int32	maxPlayerCount;
		String<kMaxGameNameLength>	gameName;
		//ResourceName	worldName;

		ServerInfoMessage();

	public:

		ServerInfoMessage(int32 numPlayers, int32 maxPlayers, const char *game);
		~ServerInfoMessage();

		int32 GetPlayerCount(void) const
		{
			return playerCount;
		}

		int32 GetMaxPlayerCount(void) const
		{
			return maxPlayerCount;
		}

		const char *GetGameName(void) const
		{
			return gameName;
		}


		void Compress(Compressor& data) const override;
		bool Decompress(Decompressor& data) override;

	};

	class RequestMessage : public Message
	{
	public:

		RequestMessage();
		~RequestMessage();

		void Compress(Compressor &data) const;
		bool Decompress(Decompressor &data);
	};

	class SpawnMessage : public Message
	{
	private:
		Point3D loc;
		int32 contIndex;
		PlayerKey key;

	public:
		SpawnMessage();
		SpawnMessage(PlayerKey playerKey, int32 controllerIndex, Point3D location);
		~SpawnMessage();

		bool HandleMessage(Player *sender) const;

		void Compress(Compressor &data) const;
		bool Decompress(Decompressor &data);

		Point3D GetLocation() const
		{
			return loc;
		}

		PlayerKey GetPlayerKey() const
		{
			return key;
		}

		long GetControllerIndex() const
		{
			return contIndex;
		}
	};

	class ClientMovementMessage : public Message
	{
		friend class Game;

	private:

		unsigned long movementFlag;

		ClientMovementMessage(MessageType type);

	public:

		ClientMovementMessage(MessageType type, unsigned long flag);
		~ClientMovementMessage();

		long GetMovementFlag(void) const
		{
			return movementFlag;
		}

		void Compress(Compressor& data) const;
		bool Decompress(Decompressor& data);

		bool HandleMessage(Player *sender) const;
	};

	class ClientOrientationMessage : public Message
	{
	private:

		float orientation;

	public:

		ClientOrientationMessage(void);
		ClientOrientationMessage(float _orientation);
		~ClientOrientationMessage(void);

		void Compress(Compressor& data) const;
		bool Decompress(Decompressor& data);

		bool HandleMessage(Player *sender) const;
	};

	class LoadMultiplayerLevel : public Message
	{
		private:
			unsigned long level;
		public:
			LoadMultiplayerLevel();
			LoadMultiplayerLevel(unsigned long lvl);
			~LoadMultiplayerLevel();

			unsigned long GetMultiplayerLevel()
			{
				return level;
			}

			void Compress(Compressor& data) const;
			bool Decompress(Decompressor& data);

	};
}