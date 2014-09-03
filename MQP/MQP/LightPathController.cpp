#include <cmath>

#include "LightPathController.h"
#include "Game.h"

using namespace C4;

LightPathController::LightPathController() : Controller(kControllerLightPath)
{
	pitch = 0.0f;
	roll = 0.0f;
	yaw = 0.0f;
	nextPitch = 0.0f;
	nextRoll = 0.0f;
	nextYaw = 0.0f;
	targetRoll = 0.0f;
	changed = true;
	hand = nullptr;
	player = nullptr;
	distance = 0.0f;
	speed = START_SPEED;
	firstFrame = true;
	switchObject = nullptr;
	lead = true;
	switchTimer = 0;
	splineTimer = 0.0f;
}

LightPathController::LightPathController(const LightPathController& lightPathController) :
Controller(lightPathController)
{
}

Controller* LightPathController::Replicate(void) const
{
	return (new LightPathController(*this));
}

LightPathController::~LightPathController()
{
	
}

bool LightPathController::ValidNode(const Node* node)
{
	return ((node) && (node->GetNodeType() == kNodeGeometry));
}

void LightPathController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void LightPathController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

//Find hand node and set controller
void LightPathController::Preprocess(void)
{
	Controller::Preprocess();

	// Start with collision detection off
	((GeometryObject*)GetTargetNode()->GetObject())->SetCollisionExclusionMask(kCollisionExcludeAll);
}

void LightPathController::Move(void)
{
	if (lead)
	{
		// Make sure this has hand and controller
		if ((hand == nullptr) || (player == nullptr))
		{
			Node* root = GetTargetNode()->GetRootNode();
			Node* node = root;
			do
			{
				if (node->GetController())
				{
					if (node->GetController()->GetControllerType() == kControllerAnimatedHand)
					{
						hand = (HandController*)(node->GetController());
						hand->SetLightPath(this);
					}

					if (node->GetController()->GetControllerType() == kControllerPlayer)
					{
						player = (MainPlayerController*)(node->GetController());
					}
				}
				node = root->GetNextNode(node);
			} while (node);
		}

		// Get speed
		speed = 5.0F * player->GetSpeed();

		// Scale forward
		if (!firstFrame)
		{
			distance += speed * (TheTimeMgr->GetFloatDeltaTime() / 1000.0f);
		}
		Matrix3D stretch;
		stretch.Set(Vector3D(distance, 0.0f, 0.0f),
			Vector3D(0.0f, 1.0f, 0.0f),
			Vector3D(0.0f, 0.0f, 1.0f));

		// Rotation
		if (changed)
		{
			Matrix3D pitchM;
			pitchM.SetRotationAboutY(-1.0f * pitch);
			Matrix3D rollM;
			rollM.SetRotationAboutX(roll);
			Matrix3D yawM;
			yawM.SetRotationAboutZ(yaw);
			rotation = yawM * pitchM * rollM;

			changed = false;
		}

		GetTargetNode()->SetNodeMatrix3D(rotation * stretch);
		GetTargetNode()->Invalidate();

		if (!firstFrame)
		{
			// Tell player where front is
			player->ReportLightpathFront(GetTargetNode()->GetFirstSubnode()->GetWorldPosition());

			// Update next roll
			if (nextRoll < targetRoll)
			{
				nextRoll += ROLL_RATE * TheTimeMgr->GetFloatDeltaTime() / 1000.0f;
			}
			else if (nextRoll > targetRoll)
			{
				nextRoll -= ROLL_RATE * TheTimeMgr->GetFloatDeltaTime() / 1000.0f;
			}

			// Add position to player spline if it's time
			splineTimer += TheTimeMgr->GetFloatDeltaTime();
			if (splineTimer >= SPLINE_FREQUENCY)
			{
				if (player)
				{
					player->SplinePoint(GetTargetNode()->GetFirstSubnode()->GetWorldPosition());
					splineTimer = 0.0f;
				}
			}
		}
		firstFrame = false;

		// Move to next piece if it's time
		if ((abs(nextPitch - pitch) >= PITCH_THRESHOLD) ||
			(abs(nextRoll - roll) >= ROLL_THRESHOLD) ||
			(abs(nextYaw - yaw) >= YAW_THRESHOLD))
		{	
			// Set up switch object if it doesn't already exist
			if (switchObject == nullptr)
			{
				Geometry* geometry = (Geometry*)GetTargetNode();
				GenericGeometryObject* obj = (GenericGeometryObject*)geometry->GetObject();
				switchObject = new GenericGeometryObject(geometry);
				memcpy((void*)switchObject, (void*)obj, sizeof(GenericGeometryObject));
				switchObject->SetCollisionExclusionMask(0);
			}

			// Create next piece of the path
			Node* next = GetTargetNode()->Clone();
			next->SetController(new LightPathController());

			// Set position
			float horizDistance = distance * 0.2 * cos(pitch);
			next->SetNodePosition(GetTargetNode()->GetNodePosition() +
				Point3D(horizDistance * cos(yaw),
				horizDistance * sin(yaw),
				distance * 0.2 * sin(pitch)));

			// Set rotation
			LightPathController* nextController = (LightPathController*)(next->GetController());
			nextController->SetPitch(nextPitch);
			nextController->SetRoll(nextRoll);
			nextController->SetYaw(nextYaw);
			nextController->SetSpeed(speed);
			nextController->SetHand(hand);
			nextController->SetPlayer(player);
			nextController->SetSwitchObject(switchObject);

			if (hand)
			{
				hand->SetLightPath(nextController);
			}

			//send player information 
			if (player)
			{
				if (splineTimer != 0.0f)
				{
					player->SplinePoint(GetTargetNode()->GetFirstSubnode()->GetWorldPosition());
				}
			}

			GetTargetNode()->GetRootNode()->AddNewSubnode(next);
			next->Update();

			// This is no longer the front piece
			lead = false;
		}
	}
	else
	{
		switchTimer += TheTimeMgr->GetDeltaTime();
		if (switchTimer >= SOLIDIFY_TIME)
		{
			GetTargetNode()->SetObject(switchObject);
			GetTargetNode()->Invalidate();

			// Remove controller from this node
			GetTargetNode()->SetController(nullptr);
			delete this;
		}
	}
}

void LightPathController::SetPitch(float pitch)
{
	this->pitch = pitch;
	nextPitch = pitch;
	changed = true;
}

void LightPathController::SetRoll(float roll)
{
	this->roll = roll;
	nextRoll = roll;
	changed = true;
}

void LightPathController::SetYaw(float yaw)
{
	this->yaw = yaw;
	nextYaw = yaw;
	changed = true;
}

void LightPathController::SetSpeed(float speed)
{
	this->speed = speed;
}

void LightPathController::SetHand(HandController* hand)
{
	this->hand = hand;
}

void LightPathController::SetPlayer(MainPlayerController* player)
{
	this->player = player;
}

void LightPathController::SetSwitchObject(GenericGeometryObject* switchObject)
{
	this->switchObject = switchObject;
}

void LightPathController::ChangePitch(float pitch)
{
	nextPitch = pitch;
	if (nextPitch > K::pi_over_2)
	{
		nextPitch = K::pi_over_2;
	}
	if (nextPitch < (-1.0f * K::pi_over_2))
	{
		nextPitch = (-1.0f * K::pi_over_2);
	}
}

void LightPathController::ChangeRoll(float roll)
{
	targetRoll = roll;
	if (targetRoll > K::pi_over_2)
	{
		targetRoll = K::pi_over_2;
	}
	if (targetRoll < (-1.0f * K::pi_over_2))
	{
		targetRoll = (-1.0f * K::pi_over_2);
	}
}

void LightPathController::ChangeYaw(float change)
{
	nextYaw += change;
}

Point3D LightPathController::GetFrontLeft()
{
	Transform4D offset;
	offset.SetTranslation(Point3D(0.0f, PATH_WIDTH / 2.0f, 0.0f) + GetTargetNode()->GetFirstSubnode()->GetNodePosition());
	Transform4D leftFront = GetTargetNode()->GetNodeTransform() * offset;
	return leftFront.GetTranslation();
}

Point3D LightPathController::GetFrontRight()
{
	Transform4D offset;
	offset.SetTranslation(Point3D(0.0f, -1.0f * PATH_WIDTH / 2.0f, 0.0f) + GetTargetNode()->GetFirstSubnode()->GetNodePosition());
	Transform4D rightFront = GetTargetNode()->GetNodeTransform() * offset;
	return rightFront.GetTranslation();
}