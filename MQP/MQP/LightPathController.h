#pragma once

#include "C4Controller.h"
#include "C4World.h"

#include "HandController.h"

#define START_SPEED (5.0f) // m/s
#define MIN_SPEED (1.0f)
#define MAX_SPEED (15.0f)
#define BASE_SPEED (7.0f) // base acceleration happens if speed is lower than this
#define HILL_ACCELERATION (0.001f)
#define BASE_ACCELERATION (0.0005f)
#define BASE_PITCH (0.2f) // pitch must be less than this for base acceleration
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

		float pitch;
		float roll;
		float yaw;
		bool changed;
		Matrix3D rotation;
		float nextPitch;
		float nextRoll;
		float nextYaw;
		float distance;
		float speed;

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
		void SetSpeed(float speed);

		void ChangePitch(float pitch);
		void ChangeRoll(float roll);
		void ChangeYaw(float change);
	};
}
