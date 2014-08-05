#include "Player.h"

using namespace C4;


MainPlayerController::MainPlayerController(float azimuth) :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}
 MainPlayerController::MainPlayerController() :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	 speed = START_SPEED;
}

 MainPlayerController::MainPlayerController(const MainPlayerController& playerController) :
CharacterController(playerController),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;

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

 void MainPlayerController::Move(void)
{
	 float x = 0;
	 float y = 0;
	 float z = 0;
	 float distance = 1;
	//This is called once per frame to allow the controller to 
	//move its target node.
	 if (lightPathNodes.empty()){
		 //go straight
		 x += 0.5;
		 y += 0.5;
		 z += 0.5;
	 }
	 else{
		 
		 Point3D lpos = lightPathNodes.front()->GetNodePosition();
		 Point3D ppos = GetTargetNode()->GetNodePosition();
		 distance = std::sqrt(pow(lpos.x - ppos.x, 2) + pow(lpos.y - ppos.y, 2) + pow(lpos.z - ppos.z, 2));
		 while (distance < 0.2f){
			 //POP
			 lightPathNodes.pop();
			 //check if empty
			 if (lightPathNodes.empty()){
				 break;
			 }
			 else {
				 lpos = lightPathNodes.front()->GetNodePosition();
				 distance = std::sqrt(pow(lpos.x - ppos.x, 2) + pow(lpos.y - ppos.y, 2) + pow(lpos.z - ppos.z, 2));

				 // Face forwards
				 float rotx = 0;
				 float roty = 0;
				 float rotz = 0;
				 lightPathNodes.front()->GetNodeTransform().GetEulerAngles(&rotx, &roty, &rotz);
				 Matrix3D rotation;
				 rotation.SetEulerAngles(rotx, roty, rotz);
				 GetTargetNode()->SetNodeMatrix3D(rotation);
			 }
		 }
		 float vt = (speed * TheTimeMgr->GetFloatDeltaTime()) / 1000.0F;
		 x = ((lpos.x - ppos.x) / distance) * vt + ppos.x;
		 y = ((lpos.y - ppos.y) / distance) * vt + ppos.y;
		 z = ((lpos.z - ppos.z) / distance) * vt + ppos.z;
	 }

	 /*GetTargetNode()->SetNodePosition(Point3D(GetTargetNode()->GetNodePosition().x + speed * (TheTimeMgr->GetFloatDeltaTime()/1000.0F)
		 ,0
		 ,0));*/
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
		return Point3D(0.0f, 0.0f, 0.0f);
	}
	else
	{
		return lightPathNodes.front()->GetNodePosition();
	}
}