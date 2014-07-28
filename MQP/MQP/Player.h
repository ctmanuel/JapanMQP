
#ifndef PLAYER H
#define PLAYER H

#include "C4Controller.h"
#include "C4World.h"
#include "Leap.h"

namespace C4
{
	enum
	{
		kControllerPlayer = 'Play'
	};

	class PlayerController : public Controller
	{
	private:
		PlayerController(const PlayerController& playerController);				//private constructor

		Controller *Replicate(void) const;

		Node *PlayerNode;														//Player's node

	public:

		//public constructor
		PlayerController();
		//destructor
		~PlayerController();

		static bool ValidNode(const Node *node);

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);
		void Move(void);

		Point3D PlayerPosition();
	};
}

#endif
