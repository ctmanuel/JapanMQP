#pragma once

#include "C4Cameras.h"
#include "C4World.h"

namespace C4
{
	class GameCamera : public FrustumCamera
	{

	public:
		GameCamera();
		~GameCamera();

		void Preprocess(void);
		void Move(void);
	};
}
