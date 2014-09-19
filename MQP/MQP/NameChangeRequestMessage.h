#pragma once

#include "C4Messages.h"

namespace C4
{
	//unique identifier for our NameChangeREquestMessage so the program can
	//determine whether the message it recieves is a NameChangeRequestMessage or not
	enum
	{
		kMessageServerInfo = kMessageBaseCount,
		//identifier for our name change request message
		kMessageNameChangeRequestMessage,

		//This is the identifier for our name change confirmation message
		kMessageNameChangeMessage
	};

	class NameChangeRequestMessage : public Message
	{
	private:

		String<kMaxPlayerNameLength>	newName;

	public:

		NameChangeRequestMessage();

		//only want to set a new players name through the constructor of the message
		NameChangeRequestMessage(const char *name);
		NameChangeRequestMessage(MessageType type);
		NameChangeRequestMessage(MessageType type, const char *name);

		~NameChangeRequestMessage();

		//get new name from the object through getter method
		const char* GetNewName(void) const
		{
			return newName;
		}

		//insert and extract the data contained in a data package and put them into class variables
		void Compress(Compressor &data) const;
		bool Decompress(Decompressor &data);
	};

	class NameChangeMessage : public NameChangeRequestMessage
	{
	private:
		//We'll only store the players Key, which is unique to each player on a server
		PlayerKey playerKey;
	public:
		//The constructors are the only wat to set the new name and the new player key
		//The constructors in the NameChangeMessage message type call the parent constructor
		//(that of NameChangeRequestMessage) with the MessageType of the NameChangeMessage.
		NameChangeMessage();
		NameChangeMessage(const char* name, PlayerKey pk);
		~NameChangeMessage();

		//The compress and decompress methods
		void Compress(Compressor &data) const;
		bool Decompress(Decompressor &data);

		//Simple accessor for our player key valus
		const PlayerKey GetPlayerKey() const {
			return playerKey;
		}
	};
}