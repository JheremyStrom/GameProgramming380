#pragma once
#include "Actor.h"
class Player :
	public Actor
{
public:
	Player(class Game*);
	~Player();

	void Respawn();

	void SetRespawnPos(Vector3 respawnPos) { mRespawnPos = respawnPos; }

private:
	Vector3 mRespawnPos;
};

