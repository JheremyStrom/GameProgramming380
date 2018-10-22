#include "Player.h"
#include "Game.h"
#include "PlayerMove.h"
#include "CollisionComponent.h"
#include "CameraComponent.h"

Player::Player(Game* game) : Actor(game)
{
	mMove = new PlayerMove(this);

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(50.0f, 175.0f, 50.0f);

	mCamera = new CameraComponent(this);

	mGame->SetPlayer(this);
}


Player::~Player()
{
}

void Player::Respawn() {
	mPosition = mRespawnPos;
	mRotation = 0.0f;
}
