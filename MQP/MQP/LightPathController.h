#pragma once

#include "C4Controller.h"
#include "C4World.h"

#include "HandController.h"
#include "Player.h"

#define PITCH_THRESHOLD (0.05f)
#define ROLL_THRESHOLD (0.1f)
#define YAW_THRESHOLD (0.05f)
#define ROLL_RATE (1.0f) // radians/second
#define SOLIDIFY_TIME (4000) // msec it takes for a piece of the path to turn on collision detection after it's not the front piece
#define SPLINE_FREQUENCY (50.0f) // msec before adding front of path to player spline
#define PATH_WIDTH (1.0f) // m

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
		float splineTimer;

		HandController* hand;
		MainPlayerController* player;

		bool firstFrame;

		bool lead;
		GenericGeometryObject* switchObject;
		int switchTimer;

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
		void SetSwitchObject(GenericGeometryObject* switchObject);

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
		void FindPlayer();

		Point3D GetFrontLeft();
		Point3D GetFrontRight();
	};
}
