
#ifndef _PLAYER_H_
 #define _PLAYER_H_

#include "C4Controller.h"
#include "C4Character.h"
#include "C4World.h"
#include "C4Engine.h"

#include "Leap.h"

#include "PlayerInteractor.h"
#include "LightPathController.h"

#include "spline_library\hermite\cubic\cr_spline.h"
#include "spline_library\splinelengthcalculator.h"

#include <vector>

// Spline stuff
#define MAX_SPLINE_POINTS (50)
#define DISTANCE_TO_PATH (2.0F)
#define DISTANCE_TO_PATH (2.0F) // meters between player and front of light path

// Speed stuff
#define START_SPEED (1.0f) // m/s
#define MIN_SPEED (10.0f) // m/s
#define MAX_SPEED (25.0f) // m/s
#define BASE_SPEED (5.0f) // m/s, if below this speed, player will gradually accelerate towards it
#define BASE_ACCELERATION (0.5f) // m/s/s
#define BASE_CLIMB_THRESHOLD (2.0f) // m/s vertical that player must be below to get base acceleration
#define HILL_ACCELERATION (0.1f) // no useful units. just a scale
#define TURN_SLOW_THRESHOLD (0.1f) // turn "sharpness" (in no useful units) above which player will lose speed when turning
#define TURN_ACCELERATION (1.0f) // m/s/s
#define ROLL_REQUIREMENT (0.6f) // roll required to not lose speed around turns


namespace C4
{
	enum
	{
		 kControllerPlayer = 'plr'
	};

	enum
	{
		 kModelPlayer = 'plr'
	};

	enum PowerUp
	{
		powerUpNone,
		powerUpSpeedBoost,
		powerUpRingExpander
	};

	class MainPlayerController;
	
	class MainPlayer : public Player
	{
	private:
			
		ControllerReg<MainPlayerController>		playerControllerRegistration;
		ModelRegistration						playerModelRegistration;
			
		MainPlayer();
		~MainPlayer();
			
	public:
					
		static void Construct(void);
		static void Destruct(void);
	};
	
		
	//Controllers are used to control anything that moves in the world.
	//new types of controllers defined by the application/game module can be
	//registered with the engine when the Game class is constructed.
	//
	//This controlleris used to animate the player. It uses the built-in 
	//character controller as a base class so that the engine's native 
	//physics can be used to move the character
	class MainPlayerController : public CharacterController
	{
	private:
		//motion states that are used to keep track 
		//of which animation should be played
			
		enum
		{
		kMotionNone,
		kMotionStand,
		kMotionForward,
		};
		
		//movement flags tell how the user is trying to move the  player
		unsigned_int32		movementFlags;

		//player motion keeps track of what animation is currently playing
			
		int32				playerMotion;
		
		//The azimuth and altitude represent the direction the player is looking 
		//by using the rift
			
		float				modelAzimuth;
		float				modelAltitude;
		
		//frame animator controls playback of an animation resource
			
		FrameAnimator		frameAnimator;
		
		//Previous center of mass stores the center point of the character on the
		//previous frame. This is used with the new center point to activate triggers
			
		Point3D				previousCenterOfMass;

		Vector3D			direction = Vector3D(0.0f,0.0f,0.0f);
		
		//we keep an interactor object here in the controller
		PlayerInteractor	playerInteractor;

		//spline stuff
		std::vector<SplineVector3D> splinePoints;
		SplineVector3D		lightPathFront;

		//player speed
		float speed = START_SPEED;
		//presious speed
		float prevSpeed = 0;

		std::vector<Node*> ringNodes;

		std::vector<float>	rollHistory;

		MainPlayerController(const MainPlayerController& playerController);				//private constructor
		
		Controller *Replicate(void) const override;
		
		void SetPlayerMotion(int32 motion);

		long levelTime;
		float speedTime = 0;				//duration of the spped item

		PowerUp powerUp;

		Sound* pathSound;
		Sound* bankSound;

		bool banking;

	public:

		//public constructor
		MainPlayerController(float azimuth);
		
		MainPlayerController();
		
		//destructor
		~MainPlayerController();

		Model *GetTargetNode(void) const
		{
			return (static_cast<Model *>(Controller::GetTargetNode()));
		}

		unsigned_int32 GetMovementFlags(void) const
		{
			return (movementFlags);
		}

		void SetMovementFlags(unsigned_int32 flags)
		{
			movementFlags = flags;
		}

		float GetModelAzimuth(void) const
		{
			return (modelAzimuth);
		}
		
		float GetModelAltitude(void) const
		{
			return (modelAltitude);
		}

		PlayerInteractor *GetPlayerInteractor(void)
		{
			return (&playerInteractor);
		}

		float GetSpeed()
		{
			return speed;
		}

		Vector3D GetDirection()
		{
			return direction;
		}
		
		static bool ValidNode(const Node *node);

		void SplinePoint(Point3D position);
		void LightpathSpeed(float speed);
		void ReportLightpathFront(Point3D front);
		void ReportRoll(float roll);
		Point3D GetLightPathFront(void);
		void AddSpeed(float speedChange);
		PowerUp GetPowerUp(void);
		void SetPowerUp(PowerUp powerUp);
		void UsePowerUp(void);
		
		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);
		
		void Preprocess(void);
		void Move(void);

		RigidBodyStatus HandleNewGeometryContact(const GeometryContact* contact);
		RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact* contact, RigidBodyController* contactBody);
	};
}

#endif
