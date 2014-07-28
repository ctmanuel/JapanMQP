
#ifndef PLAYER_H
#define PLAYER_H

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
		//private constructor
		PlayerController(const PlayerController& playerController);

		Controller *Replicate(void) const;

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
	};
}

#endif
