#ifndef _PLAYER_INTERACTOR_H_
 #define _PLAYER_INTERACTOR_H_

#include "C4World.h"
 #include "C4Engine.h"

namespace C4
 {
	class MainPlayerController;
	
		class PlayerInteractor : public Interactor
		 {
		private:
			
				MainPlayerController* playerController;
			
				public:
					
						PlayerInteractor(MainPlayerController *controller);
					
						PlayerInteractor();
					
						~PlayerInteractor();
					
						void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position) override;
					
						};
	}

#endif
