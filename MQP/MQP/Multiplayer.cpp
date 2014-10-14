#include "Multiplayer.h"
#include "HandController.h"


using namespace C4;

GamePlayer::GamePlayer(PlayerKey key) : Player(key)
{
	cont = nullptr;
}

GamePlayer::~GamePlayer()
{
}

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

RequestMessage::RequestMessage() : Message(kMessageRequest)
{
}

RequestMessage::~RequestMessage()
{
}

void RequestMessage::Compress(Compressor &data) const
{
}

bool RequestMessage::Decompress(Decompressor &data)
{
	return true;
}

SpawnMessage::SpawnMessage() : Message(kMessageSpawn)
{
	this->contIndex = 0;
	this->key = -1;
}

SpawnMessage::SpawnMessage(PlayerKey playerkey, int32 controllerIndex, Point3D location) : Message(kMessageSpawn)
{
	this->contIndex = controllerIndex;
	this->key = playerkey;
	this->loc = location;
}

SpawnMessage::~SpawnMessage()
{

}

bool SpawnMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetPlayer(GetPlayerKey());
	TheGame->SpawnPlayer(player, GetLocation(), GetControllerIndex());
	
	return true;
}

void SpawnMessage::Compress(Compressor &data) const
{
	data << loc;
	data << contIndex;
	data << key;
}

bool SpawnMessage::Decompress(Decompressor &data)
{
	data >> loc;
	data >> contIndex;
	data >> key;

	return (true);
}

ClientMovementMessage::ClientMovementMessage(MessageType type) : Message(type)
{
}

ClientMovementMessage::ClientMovementMessage(MessageType type, unsigned long flag) : Message(type)
{
	movementFlag = flag;
}

ClientMovementMessage::~ClientMovementMessage()
{
}

void ClientMovementMessage::Compress(Compressor& data) const
{
	data << (unsigned char)movementFlag;
}

bool ClientMovementMessage::Decompress(Decompressor& data)
{
	unsigned char	flag;

	data >> flag;
	movementFlag = flag;

	return (true);
}

bool ClientMovementMessage::HandleMessage(Player *sender) const
{
	//TODO:: Get player controller
	return true;
}

ClientOrientationMessage::ClientOrientationMessage(void) : Message(kMessageOrientation)
{
}

ClientOrientationMessage::ClientOrientationMessage(float _orientation) : Message(kMessageOrientation)
{
	orientation = _orientation;
}

ClientOrientationMessage::~ClientOrientationMessage()
{
}

void ClientOrientationMessage::Compress(Compressor& data) const
{
	data << orientation;
}

bool ClientOrientationMessage::Decompress(Decompressor& data)
{
	data >> orientation;
	return (true);
}

bool ClientOrientationMessage::HandleMessage(Player *sender) const
{
	//SoldierController *controller = static_cast<GamePlayer *>(sender)->GetController();
	//if (controller)
	//{
	//	controller->BeginOrientation(orientation);
	//}

	return (true);
}

LoadMultiplayerLevel::LoadMultiplayerLevel() : Message(kMessageBattleLevel)
{

}

LoadMultiplayerLevel::LoadMultiplayerLevel(unsigned long lvl) : Message(kMessageBattleLevel)
{
	level = lvl;
}

LoadMultiplayerLevel::~LoadMultiplayerLevel()
{

}

void LoadMultiplayerLevel::Compress(Compressor& data) const
{
	data << (unsigned char) level;
}

bool LoadMultiplayerLevel::Decompress(Decompressor& data)
{
	unsigned char	lvl;
	data >> lvl;
	level = lvl;

	return true;
}
