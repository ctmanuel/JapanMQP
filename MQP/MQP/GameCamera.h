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

	public:
		GameCamera();
		~GameCamera();

		Model *GetTargetModel(void) const
		{
			return (playerModel);
		}

		void SetTargetModel(Model *model)
		{
			playerModel = model;
		}

		void Preprocess(void);
		void Move(void);
		void Reset(void);
	};
}
