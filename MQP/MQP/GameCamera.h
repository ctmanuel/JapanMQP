#pragma once

#include "C4Cameras.h"
#include "C4World.h"

namespace C4
{
	class GameCamera : public FrustumCamera
	{

	private:
		Quaternion origin;
		Model *playerModel;
		Node *playerNode;
		Node *GetPlayerNode();
		bool lookedForPlayer;

	public:
		GameCamera();
		~GameCamera();

		void Preprocess(void);
		void Move(void);
		void Reset(void);

	};
}
