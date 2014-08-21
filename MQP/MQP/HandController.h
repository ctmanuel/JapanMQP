#pragma once

#include "C4Controller.h"
#include "C4World.h"
#include "C4Animation.h"
#include "C4Models.h"
#include "Leap.h"

#include "LightPathController.h"
#include "Player.h"
#include "LightParticleSystem.h"

#define Z_MID 225.0f
#define PITCH_SENSITIVITY 2.5f
#define ROLL_SENSITIVITY 1.0f
#define YAW_SENSITIVITY 0.0025f
#define ROLL_REPORT_FREQUENCY 100 // milliseconds between roll reports to player


namespace C4
{
	enum 
	{
		kControllerHand = 'hand',
		kControllerMenuHand = 'mhnd'
	};

	///Defines the type of model to be loaded.
	enum
	{
		kModelAnimatedHand = 'ahnd'
	};

	///This defines the type of controller that is goign to be associated with teh animated object.
	enum
	{
		kControllerAnimatedHand = 'anhd'
	};

	class LightPathController;
	class HandController;

	//The interactor class is used to track player interactions with objects in the scene.
	class HandInteractor : public Interactor
	{
		private:

			HandController	*handController;

		public:

			HandInteractor(HandController *controller);
			HandInteractor();
			~HandInteractor();

			void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position) override;
	};
	
	class HandController : public CharacterController
	{
	private:

		Leap::Controller leap;

		Matrix3D startOrientation;

		HandInteractor handInteractor;

		HandController(const HandController& handController);
		Controller* Replicate(void) const;

		//Indicates whether the animation should be played backwards(true) or not (false).
		bool backward;

		///Attribute responsible for controllign the model animation.
		FrameAnimator		*frameAnimator;

		///Path to the location of the model containing the animation to be used by 
		///the model the controller will be attached to.
		String<> animatedModelPath;

		LightPathController* lightPath;
		MainPlayerController* player;

		int rollTimer;

		// Light particle system
		LightParticleSystem* lps;

	public:

		HandController();

		//Class constructor with float representing the boolean value that sets the value of the backward attribute
		//and path to the model containing animation to be used by the controller.
		//\param amp Path to the model conatinign the animation to be used.
		//\param b Sets cyclic direction of the animation. If the value of is true, backward direction is used. If 
		//         false, forward direction is used.
		HandController(const char* amp, bool b);
		~HandController();

		//Serialization function used when packing and sending a message.
		void Pack(Packer& data, unsigned long packFlags) const;

		//Inverse serialization function used when receiveing and opening a message.
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		//This method is called once during the instance creation.
		void Preprocess(void);

		//The function that udates the controller.
		void Move(void);

		//Sets the light path based on hand orientation
		void SetLightPath(LightPathController* lightPath);

		//Returns the value of the backward attribute.
		//\return bool Returns true if animation is looping backward and false otherwise.
		bool GetBackward(void) const
		{
			return backward;
		}

		//Sets the value of the backward attribute.
		//\param b Sets the animation to loop forward (false) or backward (true).
		void SetBackward(bool b)
		{
			backward = b;
		}

		//Checks if the node associated with controller is of valid type.
		//\param node Node to be evaluated.
		//\return bool Ture if the node is of valid type and false otherwise.
		static bool ValidNode(const Node *node);

		//Returns this nodes model
		Model *GetTargetModel(void) const
		{
			return (static_cast<Model *>(Controller::GetTargetNode()));
		}

		//Return Hand Interactor
		HandInteractor *GetHandInteractor(void)
		{
			return (&handInteractor);
		}

		//Is called whenever node interacts with a geometry node
		RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);
	};

//---------------------------Menu stuff-------------------------------------------------

	class MenuHandController;

	class MenuHandInteractor : public Interactor
	{
	private:
		MenuHandController* controller;

	public:
		MenuHandInteractor(MenuHandController* controller);
		~MenuHandInteractor();

		void HandleInteractionEvent(InteractionEventType type, Node* node, const Point3D* position) override;
	};

	class MenuHandController : public Controller
	{
	private:
		Leap::Controller leap;

		bool pushed;
		int firstFrame;

		MenuHandInteractor interactor;

	public:
		MenuHandController();
		~MenuHandController();

		void Preprocess(void);
		void Move(void);
	};
}

