#pragma once

#include "C4Controller.h"
#include "C4World.h"

#include "HandController.h"

#define SPEED 0.005f
#define PITCH_THRESHOLD (0.1f)
#define ROLL_THRESHOLD (0.1f)
#define YAW_THRESHOLD (0.1f)

namespace C4
{
	enum
	{
		kControllerLightPath = 'lpth'
	};

	class HandController;

	class LightPathController : public Controller
	{
	private:

		LightPathController(const LightPathController& lightPathController);
		Controller* Replicate(void) const;

		int timer;
		float pitch;
		float roll;
		float yaw;
		bool changed;
		Matrix3D rotation;
		float nextPitch;
		float nextRoll;
		float nextYaw = 0.0f;

		HandController* hand;

	public:
		LightPathController();
		~LightPathController();

		static bool ValidNode(const Node* node);

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);

		void Move(void);

		void SetPitch(float pitch);
		void SetRoll(float roll);
		void SetYaw(float yaw);

		void ChangePitch(float pitch);
		void ChangeRoll(float roll);
		void ChangeYaw(float change);
	};
}
