#pragma once

#include "C4Controller.h"
#include "C4World.h"

namespace C4
{
	enum
	{
		kControllerTank = 'tank'
	};
	enum
	{
		kModelTank = 'tank'
	};
	class TankController : public Controller
	{
	private:

		TankController(const TankController& tankController);

		Controller* Replicate(void) const;

	public:

		TankController();
		~TankController();

		static bool ValidNode(const Node* node);

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);
		void Move(void);
	};
}
