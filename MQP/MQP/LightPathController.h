#pragma once

#include "C4Controller.h"
#include "C4World.h"

#include "HandController.h"
#include "Player.h"

#define PITCH_THRESHOLD (0.1f)
#define ROLL_THRESHOLD (0.04f)
#define YAW_THRESHOLD (0.1f)

#define ROLL_RATE (1.0f) // radians/second

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
		float targetRoll;
		float distance;
		float speed;

		HandController* hand;
		MainPlayerController* player;

		bool firstFrame;

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
		void SetHand(HandController* hand);
		void SetPlayer(MainPlayerController* player);

		void ChangePitch(float pitch);
		void ChangeRoll(float roll);
		void ChangeYaw(float change);

		float GetPitch(){
			return pitch;
		}
		float GetYaw(){
			return yaw;
		}
		float GetRoll(){
			return roll;
		}
		float GetSpeed(){
			return speed;
		}
	};
}
