#pragma once

#include "C4Engine.h"
#include "C4World.h"

#include "GameCamera.h"

namespace C4 
{
	class GameWorld : public World
	{
	private:

		GameCamera camera;

	public:

		GameWorld(const char* name);
		~GameWorld();

		WorldResult Preprocess(void);
		void Interact(void);
		void Render(void);
	};
}

