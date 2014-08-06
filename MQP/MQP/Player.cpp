#include "Player.h"

using namespace C4;


MainPlayerController::MainPlayerController(float azimuth) :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;
	currentPathNode = nullptr;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}
 MainPlayerController::MainPlayerController() :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	 speed = START_SPEED;
	 currentPathNode = nullptr;
}

 MainPlayerController::MainPlayerController(const MainPlayerController& playerController) :
CharacterController(playerController),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;
	currentPathNode = nullptr;

	modelAzimuth = 0.0F;
	modelAltitude = 0.0F;
}

 MainPlayerController::~MainPlayerController()
{
	
}

//make duplicate of controller with pointer to current controller
Controller *MainPlayerController::Replicate(void) const
 {
	return (new MainPlayerController(*this));
}

 bool MainPlayerController::ValidNode(const Node *node)
{
	return ((node) && (node->GetNodeType() == kNodeModel) || node->GetNodeType() == kNodeGeometry);
}

 void MainPlayerController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

 void MainPlayerController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

 void MainPlayerController::Preprocess(void)
{
		//This function is called once before the target node is ever
		//rendered or moved. The base class PreProcess() function should
		// always be called first, and then the subclass can do whatever 
		//preprocessing it needs to do.
		
		Controller::Preprocess();
}

 void MainPlayerController::LightpathNode(Node *node){
	 lightPathNodes.push(node);
 }

 float MainPlayerController::linearInterpolation(float p1, float p2, float mu){
	 return (p1*(1 - mu) + p2*mu);
 }

 void MainPlayerController::Move(void)
 {
	 //This is called once per frame to allow the controller to 
	 //move its target node.
	 float x = 0;
	 float y = 0;
	 float z = 0;
	 float distance = 1;

	 //check if on a straight path; if true go straight forward
	 if (lightPathNodes.empty()){
		 //go straight
		 if (currentPathNode)
		 {
			 // Get angles from light path
			 float pitch = ((LightPathController*)(currentPathNode->GetController()))->GetPitch();
			 float roll = ((LightPathController*)(currentPathNode->GetController()))->GetRoll();
			 float yaw = ((LightPathController*)(currentPathNode->GetController()))->GetYaw();

			 float horizDistance = (speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f) * cos(pitch);
			 x = horizDistance * cos(yaw) + GetTargetNode()->GetNodePosition().x;
			 y = horizDistance * sin(yaw) + GetTargetNode()->GetNodePosition().y;
			 z = (speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f) * sin(pitch) + GetTargetNode()->GetNodePosition().z;
		 }
		 else // At the beginning
		 {
			 x = GetTargetNode()->GetNodePosition().x + speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f;
		 }
	 }

	 else{
		 
		 Point3D lpos = lightPathNodes.front()->GetNodePosition();
		 Point3D ppos = GetTargetNode()->GetNodePosition();
		 distance = std::sqrt(pow(lpos.x - ppos.x, 2) + pow(lpos.y - ppos.y, 2) + pow(lpos.z - ppos.z, 2));

		 float vt = (speed * TheTimeMgr->GetFloatDeltaTime()) / 1000.0F;
		 /*x = ((lpos.x - ppos.x) / distance) * vt + ppos.x;
		 y = ((lpos.y - ppos.y) / distance) * vt + ppos.y;
		 z = ((lpos.z - ppos.z) / distance) * vt + ppos.z;*/
		 x = linearInterpolation(ppos.x, lpos.x, vt + 0.09F);
		 y = linearInterpolation(ppos.y, lpos.y, vt + 0.09F);
		 z = linearInterpolation(ppos.z, lpos.z, vt + 0.09F);

		 while (distance < 0.1f){
			 //if close to destination block, pop current block from lightpath queue
			 currentPathNode = lightPathNodes.front();
			 lightPathNodes.pop();

			 //check if lightpath queue is empty
			 if (lightPathNodes.empty()){
				 // Get angles from light path
				 float pitch = ((LightPathController*)(currentPathNode->GetController()))->GetPitch();
				 float roll = ((LightPathController*)(currentPathNode->GetController()))->GetRoll();
				 float yaw = ((LightPathController*)(currentPathNode->GetController()))->GetYaw();

				 float horizDistance = (speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f) * cos(pitch);
				 x = horizDistance * cos(yaw) + GetTargetNode()->GetNodePosition().x;
				 y = horizDistance * sin(yaw) + GetTargetNode()->GetNodePosition().y;
				 z = (speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f) * sin(pitch) + GetTargetNode()->GetNodePosition().z;
				 break;
			 }
			 
			 else {
				 lpos = lightPathNodes.front()->GetNodePosition();
				 distance = std::sqrt(pow(lpos.x - ppos.x, 2) + pow(lpos.y - ppos.y, 2) + pow(lpos.z - ppos.z, 2));

				 // Face forwards
				 float rotx = 0;
				 float roty = 0;
				 float rotz = 0;
				 float protx = 0;
				 float proty = 0;
				 float protz = 0;
				 lightPathNodes.front()->GetNodeTransform().GetEulerAngles(&rotx, &roty, &rotz);
				 GetTargetNode()->GetNodeTransform().GetEulerAngles(&protx, &proty, &protz);
				 Matrix3D rotation;
				 rotation.SetEulerAngles(rotx, roty, rotz);
				 GetTargetNode()->SetNodeMatrix3D(rotation);
			 }
		 }
	 }

	 GetTargetNode()->SetNodePosition(Point3D(x, y, z));
}

void MainPlayerController::SetPlayerMotion(int32 motion){
		//This function sets the animation resource corresponding to 
			//the current type of motion assigned to the player
		
		Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
	
		if (motion == kMotionStand)
		 {
		frameAnimator.SetAnimation("player/Stand");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
		}
	else if (motion == kMotionForward)
		 {
		frameAnimator.SetAnimation("player/Forward");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
		}
	
}

Point3D MainPlayerController::GetDestination()
{
	if (lightPathNodes.empty())
	{
		// Look straight
		if (currentPathNode)
		{
			// Get angles from light path
			float pitch = ((LightPathController*)(currentPathNode->GetController()))->GetPitch();
			float roll = ((LightPathController*)(currentPathNode->GetController()))->GetRoll();
			float yaw = ((LightPathController*)(currentPathNode->GetController()))->GetYaw();

			float horizDistance = (speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f) * cos(pitch);
			float x = horizDistance * cos(yaw) + GetTargetNode()->GetNodePosition().x;
			float y = horizDistance * sin(yaw) + GetTargetNode()->GetNodePosition().y;
			float z = (speed * TheTimeMgr->GetFloatDeltaTime() / 1000.0f) * sin(pitch) + GetTargetNode()->GetNodePosition().z;

			return Point3D(x, y, z);
		}
		else // At the beginning
		{
			return Point3D(GetTargetNode()->GetNodePosition().x + 1.0f, 0.0f, 1.0f);
		}
	}
	else
	{
		return lightPathNodes.front()->GetNodePosition();
	}
}