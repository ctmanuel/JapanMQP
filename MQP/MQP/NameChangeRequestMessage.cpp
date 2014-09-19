//file NameChangeRequestMessage.cpp
#include "NameChangeRequestMessage.h"

using namespace C4;

//The primary constructor tells the parent Message class what identifier the message uses.
//used in the createMessage method
NameChangeRequestMessage::NameChangeRequestMessage() : Message(kMessageNameChangeRequestMessage)
{

}
//empty constructor used in child classes. Passes the MessageType parameter upwards.
NameChangeRequestMessage::NameChangeRequestMessage(MessageType type) : Message(type)
{

}

//Simple initializing constructor for use in child classes
NameChangeRequestMessage::NameChangeRequestMessage(MessageType type, const char *name) : Message(type)
{
	newName = name;
}

//creates a new NameChangeRequest with the default NameChangeRequestMessage MessageType,
//also setting the new name field to the name indicated in the parameter
NameChangeRequestMessage::NameChangeRequestMessage(const char *name) : Message(kMessageNameChangeRequestMessage)
{
	newName = name;
}

NameChangeRequestMessage::~NameChangeRequestMessage()
{
}

//compress ahs to add out newName value to the compressor data variable passed by the engine
//to our name change request message
void NameChangeRequestMessage::Compress(Compressor &data) const
{
	data << newName;
}

//Decompress method extracts the informatino from the data variable and places it in our newName variable. 
bool NameChangeRequestMessage::Decompress(Decompressor &data)
{
	data >> newName;
	return true;
}

////////////////////////////////////////////////////////////////////

NameChangeMessage::NameChangeMessage() : NameChangeRequestMessage(kMessageNameChangeMessage)
{

}

NameChangeMessage::NameChangeMessage(const char *name, PlayerKey pk) : NameChangeRequestMessage(kMessageNameChangeMessage, name)
{
	playerKey = pk;
}

//The destructor does nothing
NameChangeMessage::~NameChangeMessage()
{
}

//We'll add the NameChangeMessage's data (the playerkey) to the data package, then
//call the parents compress method so it can add its data to the package.
void NameChangeMessage::Compress(Compressor &data) const
{
	data << playerKey;
	NameChangeMessage::Compress(data);
}

bool NameChangeMessage::Decompress(Decompressor &data)
{
	data >> playerKey;
	NameChangeMessage::Decompress(data);
	return true;
}