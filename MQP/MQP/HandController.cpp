#include "HandController.h"
#include "Game.h"

#include "string.h"

using namespace C4;

HandController::HandController() : 
		CharacterController(kControllerAnimatedHand)
{
	lightPath = nullptr;
	animatedModelPath = "Model/Gauntlet_Redux";
	backward = 0.0f;
}

HandController::HandController(const HandController& handController) : 
		CharacterController(handController)
{
	backward = 0.0f;
}

HandController::HandController(const char* amp, bool b) : 
		CharacterController(kControllerAnimatedHand)
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

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
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

	// Set up particle system
	if (!GetTargetNode()->GetManipulator()) // Check if we're in the world editor
	{
		lps = new LightParticleSystem();
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
		lps->SetEndLeft(lightPath->GetFrontLeft());
		lps->SetEndRight(lightPath->GetFrontRight());
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
		Node* parent = geometry->GetSuperNode();
		parent->PurgeSubtree();
		delete parent;
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
		sound->Delay(1);
		sound->VaryVolume((float)(TheGame->GetSoundVolume()) / 100.0f, 0);
		Sound* sound2 = new Sound;
		sound2->Load("SoundEffects/derez");
		sound2->Delay(1);
		sound2->VaryVolume((float)(TheGame->GetSoundVolume()) / 100.0f, 0);
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
			leapMotion.y = hand.stabilizedPalmPosition()[0] * -1.0f * (0.009f + (0.002f * ((float)(TheGame->GetTurnSensitivity()) / 50.0f)));
			leapMotion.z = (hand.stabilizedPalmPosition()[1] - Z_MID) * (0.009f + (0.002f * ((float)(TheGame->GetTurnSensitivity()) / 50.0f)));

			// Hand orientation
			Quaternion x, y, z;
			x.SetRotationAboutX(-1 * hand.palmNormal().roll());
			y.SetRotationAboutY(K::pi_over_2);
			z.SetRotationAboutZ(K::pi_over_2);
			GetTargetNode()->SetNodeMatrix3D((x * y * z).GetRotationMatrix());

			if (pushed)
			{
				if (hand.grabStrength() < 0.5)
				{
					pushed = false;
					const Node* interactionNode = interactor.GetInteractionNode(); 
					if (interactionNode)
					{
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
