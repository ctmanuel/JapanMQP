
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
	enum
	{
		kModelPlayer = 'Play'
	};

	class PlayerController : public Controller
	{
	private:
		PlayerController(const PlayerController& playerController);				//private constructor

		Controller *Replicate(void) const;

		Node *PlayerNode;														//Player's node
		Model *PlayerModel;														//Player model

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
		
		void SetPlayerModel(Model* model)
		{
			PlayerModel = model;
		}

		Model *GetPlayerModel()
		{
			return PlayerModel;
		}
	};
}

#endif
