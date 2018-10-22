#include "Checkpoint.h"
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Player.h"
#include "HUD.h"

Checkpoint::Checkpoint(Game* owner) : Actor(owner), active(false)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
	mMesh->SetTextureIndex(1);

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(25.0f, 25.0f, 25.0f);

	mGame->AddCheckpoint(this);
}


Checkpoint::~Checkpoint()
{

}

void Checkpoint::SetState(bool state) {
	active = state;
	if (active) {
		mMesh->SetTextureIndex(0);
	}
	else {
		mMesh->SetTextureIndex(1);
	}
}

void Checkpoint::UpdateActor(float deltaTime) {
	if (active) {
		Player* player = mGame->GetPlayer();
		if (mCollision->Intersect(player->GetCollision())) {
			if (mLevelString.size() != 0) {
				mGame->SetNextLevel(mLevelString);
			}
			if (mCheckpointText.size() != 0) {
				mGame->GetHUD()->CheckpointHit(mCheckpointText);
			}
			mState = EDead;
			SetState(false);
			mGame->RemoveCheckpoint();
			Checkpoint* checkpoint = mGame->FrontCheckpoint();
			if (checkpoint != NULL) {
				checkpoint->SetState(true);
			}
			player->SetRespawnPos(mPosition);
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Checkpoint.wav"), 0);
		}
	}
}