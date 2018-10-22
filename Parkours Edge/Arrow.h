#pragma once
#include "Actor.h"
class Arrow : public Actor
{
public:
	Arrow(class Game*);
	~Arrow();

	void UpdateActor(float deltaTime) override;
};

