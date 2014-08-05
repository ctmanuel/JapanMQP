#pragma once

#include "C4Controller.h"
#include "C4World.h"
#include "Leap.h"

#include "LightPathController.h"
#include "Player.h"

#define Z_MID 225.0f
#define PITCH_SENSITIVITY 1.0f
#define ROLL_SENSITIVITY 1.0f
#define YAW_SENSITIVITY 0.001f

namespace C4
{
	enum 
	{
		kControllerHand = 'hand'
	};
	enum
	{
		kModelGauntlet = 'gntl'
	};

	class LightPathController;

	class HandController : public Controller
	{
	private:

		Leap::Controller leap;

		Matrix3D startOrientation;

		HandController(const HandController& handController);
		Controller* Replicate(void) const;

		LightPathController* lightPath;
		MainPlayerController* player;

	public:

		HandController();
		~HandController();

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);

		void Move(void);

		void SetLightPath(LightPathController* lightPath);
	};
}

