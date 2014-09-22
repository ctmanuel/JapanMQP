#include "Multiplayer.h"

using namespace C4;

ServerInfoMessage::ServerInfoMessage() : Message(kMessageServerInfo)
{

}

ServerInfoMessage::ServerInfoMessage(int32 numPlayers, int32 maxPlayers, const char *game) : Message(kMessageServerInfo)
{
	playerCount = numPlayers;
	maxPlayerCount = maxPlayers;
	gameName = game;
}

ServerInfoMessage::~ServerInfoMessage()
{

}

void ServerInfoMessage::Compress(Compressor& data) const
{
	data << (unsigned_int16) playerCount;
	data << (unsigned_int16) maxPlayerCount;
	data << gameName;
}

bool ServerInfoMessage::Decompress(Decompressor& data)
{
	unsigned_int16 count, max;

	data >> count;
	data >> max;

	if (count > max)
	{
		return false;
	}

	playerCount = count;
	maxPlayerCount = max;

	data >> gameName;
	return true;
}