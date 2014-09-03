#include "HandController.h"
#include "Game.h"
#include "string.h"

using namespace C4;

HandInteractor::HandInteractor(HandController *controller)
{
	handController = controller;
}

HandInteractor::HandInteractor()
{
}

HandInteractor::~HandInteractor()
{
}

void HandInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position)
{
	//Always call the base class counterpart

	Interactor::HandleInteractionEvent(type, node, position);
	Engine::Report("HandInteractor Interaction Event");
	//if the node with which we are interacting has a controller, 
	// then pass the event through to that controller.

	Controller *controller = node->GetController();
	if (controller)
	{
		Engine::Report(String<63>("Node Name ") += node->GetNodeName());
		controller->HandleInteractionEvent(type, position);
		
	}
}

HandController::HandController() : 
		CharacterController(kControllerAnimatedHand),
		handInteractor(this)
{
	lightPath = nullptr;
	animatedModelPath = "Model/Gauntlet_Animated";
	backward = 0.0f;
}

HandController::HandController(const HandController& handController) : 
		CharacterController(handController),
		handInteractor(this)
{
	backward = 0.0f;
}

HandController::HandController(const char* amp, bool b) : 
		CharacterController(kControllerAnimatedHand),
		handInteractor(this)
{
	backward = b;
	animatedModelPath = amp;
}

Controller* HandController::Replicate(void) const
{
	return (new HandController(*this));
}

HandController::~HandController()
{
}

bool HandController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry || node->GetNodeType() == kNodeModel);
}

void HandController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);

	data << animatedModelPath;
	data << backward;
}

void HandController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);

	data >> animatedModelPath;
	data >> backward;
}

void HandController::Preprocess(void)
{
	CharacterController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake);
	SetFrictionCoefficient(0.0F);
	//Give it 0 gravity
	SetGravityMultiplier(0.0F);

	leapMotion = Point3D(0.0f, 0.0f, 0.0f);
	handRoll = 0.0f;

	//use frame animator to play animation resources for the hand model
	Model *myModel = GetTargetModel();
	frameAnimator = new FrameAnimator(myModel);
	myModel->SetRootAnimator(frameAnimator);
	Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
	frameAnimator->SetAnimation(animatedModelPath);
	if (backward)
		interpolator->SetMode(kInterpolatorBackward | kInterpolatorLoop);
	else
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);

	startOrientation = GetTargetNode()->GetNodeTransform().GetMatrix3D();
	Node* root = GetTargetNode()->GetRootNode();
	Node* node = root;
	do
	{
		if (node->GetController())
		{
			if (node->GetController()->GetControllerType() == kControllerPlayer)
			{
				player = (MainPlayerController*)(node->GetController());
			}
		}
		node = root->GetNextNode(node);
	} while (node);

	//Register our interactor with the World
	myModel->GetWorld()->AddInteractor(&handInteractor);

	// Set up particle system
	if (!GetTargetNode()->GetManipulator()) // Check if we're in the world editor
	{
		lps = new LightParticleSystem(Point3D(10.0f, -10.0f, 1.0f), Point3D(10.0f, 10.0f, 1.0f));
		GetTargetNode()->GetRootNode()->AddNewSubnode(lps);
	}

}

void HandController::Move(void)
{
	//animate model
	Model *myModel = GetTargetModel();
	myModel->Animate();

	Point3D basePosition(1.0f, 0.0f, 1.0f);

	if (leap.isConnected())
	{
		Leap::HandList hands = leap.frame().hands();
		if (!hands.isEmpty())
		{
			Leap::Hand hand = hands.frontmost();
			/*
			leapMotion.x = hand.stabilizedPalmPosition()[2] * -0.002f;
			leapMotion.y = hand.stabilizedPalmPosition()[0] * -0.002f;
			leapMotion.z = (hand.stabilizedPalmPosition()[1] - Z_MID) * 0.002f;
			*/
			leapMotion.x = 0.0f;
			leapMotion.y = hand.stabilizedPalmPosition()[0] * -1.0f * (0.0018f + (0.002f * ((float)(TheGame->GetTurnSensitivity()) / 50.0f)));
			leapMotion.z = (hand.stabilizedPalmPosition()[1] - Z_MID) * (0.0018f + (0.002f * ((float)(TheGame->GetTurnSensitivity()) / 50.0f)));
			if (leapMotion.y > MAX_LEAP_Y)
			{
				leapMotion.y = MAX_LEAP_Y;
			}
			if (leapMotion.y < (MAX_LEAP_Y * -1.0f))
			{
				leapMotion.y = (MAX_LEAP_Y * -1.0f);
			}
			if (leapMotion.z > MAX_LEAP_Z)
			{
				leapMotion.z = MAX_LEAP_Z;
			}
			if (leapMotion.z < (MAX_LEAP_Z * -1.0f))
			{
				leapMotion.z = (MAX_LEAP_Z * -1.0f);
			}

			handRoll = -1.0f * hand.palmNormal().roll();
			//Quaternion x;// , y, z;
			//y.SetRotationAboutY(-1 * hand.direction().pitch());
			//z.SetRotationAboutZ((-1 * hand.direction().yaw()));

			// Check for power up use
			if (hand.grabStrength() >= 1.0f)
			{
				player->UsePowerUp();
			}
		}
		else // Hand not in range of Leap
		{
			// Do something to show player that it's out of range
		}
	}

	Quaternion rollQ;
	rollQ.SetRotationAboutX(handRoll);
	GetTargetNode()->SetNodeMatrix3D(rollQ.GetRotationMatrix() * startOrientation);
	SetRigidBodyMatrix3D(rollQ.GetRotationMatrix() * startOrientation);

	SetRigidBodyPosition(basePosition + leapMotion);
	SetRigidBodyTransform(player->GetTargetNode()->GetWorldTransform() * GetTargetNode()->GetNodeTransform());
	GetTargetNode()->Invalidate();

	// Report roll to player if it's time
	rollTimer += TheTimeMgr->GetDeltaTime();
	if (rollTimer >= ROLL_REPORT_FREQUENCY)
	{
		player->ReportRoll(handRoll * ROLL_SENSITIVITY);
		rollTimer = 0;
	}

	if (lightPath)
	{
		lightPath->ChangePitch(leapMotion.z * PITCH_SENSITIVITY);
		lightPath->ChangeYaw(leapMotion.y * YAW_SENSITIVITY * (float)TheTimeMgr->GetDeltaTime());
		lightPath->ChangeRoll(handRoll * ROLL_SENSITIVITY);
	}

	// Update position of light particle system
	if (lps)
	{
		lps->SetStart(GetTargetNode()->GetWorldPosition());
		lps->SetEnd(player->GetLightPathFront());
	}
}

RigidBodyStatus HandController::HandleNewGeometryContact(const GeometryContact *contact)
{
	Geometry* geometry = contact->GetContactGeometry();
	if (geometry->GetNodeName() && Text::CompareText(geometry->GetNodeName(), "downer"))
	{
		Sound* sound = new Sound;
		sound->Load("SoundEffects/downer");
		sound->Delay(1);
		sound->VaryVolume((float)(TheGame->GetSoundVolume()) / 100.0f, 0);
		SetLinearVelocity(GetOriginalLinearVelocity());
		SetExternalLinearResistance(Vector2D(0.0F, 0.0F));
		player->AddSpeed(-2.0f);
		GetPhysicsController()->PurgeGeometryContacts(geometry);
		delete geometry;
		return (kRigidBodyContactsBroken);
	}
	else if (geometry->GetNodeName() && Text::CompareText(geometry->GetNodeName(), "speedBoost"))
	{
		GetPhysicsController()->PurgeGeometryContacts(geometry);
		delete geometry;
		player->SetPowerUp(powerUpSpeedBoost);
		return (kRigidBodyContactsBroken);
	}
	else if (geometry->GetNodeName() && Text::CompareText(geometry->GetNodeName(), "ringExpander"))
	{
		GetPhysicsController()->PurgeGeometryContacts(geometry);
		delete geometry;
		player->SetPowerUp(powerUpRingExpander);
		return (kRigidBodyContactsBroken);
	}
	else
	{
		Sound* sound = new Sound;
		sound->Load("SoundEffects/crash");
<<<<<<< HEAD
		sound->Play();
=======
		sound->Delay(1);
		sound->VaryVolume((float)(TheGame->GetSoundVolume()) / 100.0f, 0);
		Sound* sound2 = new Sound;
		sound2->Load("SoundEffects/derez");
		sound2->Delay(1);
		sound2->VaryVolume((float)(TheGame->GetSoundVolume()) / 100.0f, 0);
>>>>>>> aa26a35897d3155d63129325d805b581c1c99e56
		TheGame->SetLevelEndState(levelEndFailed);
		TheGame->StartLevel("Menu");
		return (kRigidBodyUnchanged);
	}
}

RigidBodyStatus HandController::HandleNewRigidBodyContact(const RigidBodyContact* contact, RigidBodyController* contactBody)
{
	SetLinearVelocity(Vector3D(0.0f, 0.0f, 0.0f));
	SetExternalLinearResistance(Vector2D(0.0F, 0.0F));
	return (kRigidBodyUnchanged);
}

void HandController::SetLightPath(LightPathController* lightPath)
{
	this->lightPath = lightPath;
}

//---------------------------Menu stuff-------------------------------------------------

MenuHandController::MenuHandController() :
	Controller(kControllerMenuHand),
	interactor(this)
{
	pushed = false;
	firstFrame = 3;
}

MenuHandController::~MenuHandController()
{
}

void MenuHandController::Preprocess(void)
{
	Controller::Preprocess();

	GetTargetNode()->GetWorld()->AddInteractor(&interactor);
}

void MenuHandController::Move(void)
{
	// Move back and forth

	Vector3D leapMotion = Vector3D(0.0f, 0.0f, 0.0f);
	if (leap.isConnected())
	{
		Leap::HandList hands = leap.frame().hands();
		if (!hands.isEmpty())
		{
			Leap::Hand hand = hands.frontmost();

			// Hand position
			leapMotion.x = 0.0f;
<<<<<<< HEAD
			leapMotion.y = hand.stabilizedPalmPosition()[0] * -0.01f;
			leapMotion.z = (hand.stabilizedPalmPosition()[1] - Z_MID) * 0.01f;
=======
			leapMotion.y = hand.stabilizedPalmPosition()[0] * -1.0f * (0.0018f + (0.002f * ((float)(TheGame->GetTurnSensitivity()) / 50.0f)));
			leapMotion.z = (hand.stabilizedPalmPosition()[1] - Z_MID) * (0.0018f + (0.002f * ((float)(TheGame->GetTurnSensitivity()) / 50.0f)));
>>>>>>> aa26a35897d3155d63129325d805b581c1c99e56

			// Hand orientation
			Quaternion x, y, z;
			x.SetRotationAboutX(-1 * hand.palmNormal().roll());
			y.SetRotationAboutY(K::pi_over_2);
			z.SetRotationAboutZ(K::pi_over_2);
			GetTargetNode()->SetNodeMatrix3D((x * y * z).GetRotationMatrix());

			if (pushed) // This is true on the frame LoadWorld is called
			{
<<<<<<< HEAD
				if (hand.grabStrength() < 0.2) // So is this
=======
				if (hand.grabStrength() < 0.5)
>>>>>>> aa26a35897d3155d63129325d805b581c1c99e56
				{
					pushed = false;
					// This returns a non-null but still invalid pointer. interactor is a member of the controller.
					const Node* interactionNode = interactor.GetInteractionNode(); 
					if (interactionNode)
					{
						// I try to call GetController on the invalid pointer and bad things happen
						Controller* interactionController = interactionNode->GetController(); 
						if (interactionController)
						{
							interactionController->HandleInteractionEvent(kInteractionEventDeactivate, &(interactor.GetInteractionPosition()), GetTargetNode());
						}
					}
				}
			}

			// Grip
			if (hand.grabStrength() >= 0.5)
			{
				pushed = true;
				// Send activate event
				const Node* interactionNode = interactor.GetInteractionNode();
				if (interactionNode)
				{
					Controller* interactionController = interactionNode->GetController();
					if (interactionController)
					{
						interactionController->HandleInteractionEvent(kInteractionEventActivate, &(interactor.GetInteractionPosition()), GetTargetNode());
					}
				}
			}
		}
	}

	if (firstFrame)
	{
		GetTargetNode()->SetNodePosition(Point3D(1.0f, 0.0f, 1.0f));
		firstFrame--;
	}
	else
	{
		GetTargetNode()->SetNodePosition(Point3D(1.0f, 0.0f, 1.0f) + leapMotion);
	}
	GetTargetNode()->Invalidate();

	// Update interactor
	Point3D pos = GetTargetNode()->GetNodePosition();
	interactor.SetInteractionProbe(pos, pos + Point3D(5.0f, 0.0f, 0.0f));
}

MenuHandInteractor::MenuHandInteractor(MenuHandController* controller)
{
	this->controller = controller;
}

MenuHandInteractor::~MenuHandInteractor()
{
}

void MenuHandInteractor::HandleInteractionEvent(InteractionEventType type, Node* node, const Point3D* position)
{
	Interactor::HandleInteractionEvent(type, node, position);

	Controller* controller = node->GetController();
	if (controller)
	{
		controller->HandleInteractionEvent(type, position);
	}
}
