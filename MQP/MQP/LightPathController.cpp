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
	changed = true;
	hand = nullptr;
	distance = 0.0f;
	speed = START_SPEED;
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

void LightPathController::Preprocess(void)
{
	Controller::Preprocess();

	// Find hand
	Node* root = GetTargetNode()->GetRootNode();
	Node* node = root;
	do
	{
		if (node->GetController())
		{
			if (node->GetController()->GetControllerType() == kControllerHand)
			{
				hand = (HandController*)(node->GetController());
				hand->SetLightPath(this);
			}
		}
		node = root->GetNextNode(node);
	} while (node);
}

void LightPathController::Move(void)
{
	distance += speed * (float)((float)TheTimeMgr->GetDeltaTime() / 1000.0f);

	// Scale forward
	Matrix3D stretch;
	stretch.Set(Vector3D(distance, 0.0f, 0.0f),
		Vector3D(0.0f, 1.0f, 0.0f),
		Vector3D(0.0f, 0.0f, 1.0f));

	Matrix3D identity;
	identity.SetIdentity();

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

	// Gain or lose speed depending on pitch
	speed += (-1.0f * pitch) * HILL_ACCELERATION * TheTimeMgr->GetDeltaTime();
	if (speed < MIN_SPEED)
	{
		speed = MIN_SPEED;
	}
	if (speed > MAX_SPEED)
	{
		speed = MAX_SPEED;
	}
	if ((speed < BASE_SPEED) && (pitch < BASE_PITCH))
	{
		speed += (BASE_ACCELERATION * TheTimeMgr->GetDeltaTime());
	}

	char s[64];
	sprintf(s, "Pitch: %f", pitch);
	TheEngine->Report(s);

	// Move to next piece if it's time
	if ((abs(nextPitch - pitch) >= PITCH_THRESHOLD) ||
		(abs(nextRoll - roll) >= ROLL_THRESHOLD) ||
		(abs(nextYaw - yaw) >= YAW_THRESHOLD))
	{
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

		if (hand)
		{
			hand->SetLightPath(nextController);
		}
		GetTargetNode()->GetRootNode()->AddNewSubnode(next);
		next->Update();

		// Remove controller from this node
		GetTargetNode()->SetController(nullptr);
		delete this;
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
	nextRoll = roll;
	if (nextRoll > K::pi_over_2)
	{
		nextRoll = K::pi_over_2;
	}
	if (nextRoll < (-1.0f * K::pi_over_2))
	{
		nextRoll = (-1.0f * K::pi_over_2);
	}
}

void LightPathController::ChangeYaw(float change)
{
	nextYaw += change;
}