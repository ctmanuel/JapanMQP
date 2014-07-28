#pragma once

#include "C4Controller.h"
#include "C4World.h"
#include "Leap.h"

namespace C4
{
	enum 
	{
		kControllerHand = 'hand'
	};

	class HandController : public Controller
	{
	private:

		Leap::Controller leap;

		Matrix3D startOrientation;

		HandController(const HandController& handController);
		Controller* Replicate(void) const;

	public:

		HandController();
		~HandController();

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);

		void Move(void);
	};
}

