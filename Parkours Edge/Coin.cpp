#include "Coin.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "MoveComponent.h"
#include "Game.h"
#include "HUD.h"

Coin::Coin(Game* game) : Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));

	mMove = new MoveComponent(this);

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(100.0f,100.0f,100.0f);
}

Coin::~Coin()
{
}

void Coin::UpdateActor(float deltaTime) {
	mMove->SetAngularSpeed(Math::Pi);

	if (mCollision->Intersect(mGame->GetPlayer()->GetCollision())) {
		mGame->GetHUD()->CollectedCoin();
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Coin.wav"), 0);
		SetState(EDead);
	}
}
