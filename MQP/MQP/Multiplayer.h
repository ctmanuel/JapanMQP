#pragma once

#include "C4Application.h"

using namespace C4;

enum
{
	kMessageServerInfo = kMessageBaseCount
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
