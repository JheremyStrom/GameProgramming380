#include "Arrow.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Player.h"
#include "Checkpoint.h"

// Arrow shows in game world once the last checkpoint is deleted

Arrow::Arrow(Game* game) : Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));
	
	mScale = 0.15f;

}

Arrow::~Arrow()
{
}

void Arrow::UpdateActor(float deltaTime) {
	if (mGame->FrontCheckpoint() == NULL) {  // Level is over
		mQuat = Quaternion::Identity;
		// delete arrow because it is no longer needed on this level
		mState = EDead; // delete this;
		return;
	}

	Vector3 temp = mGame->FrontCheckpoint()->GetPosition() - mGame->GetPlayer()->GetPosition();
	temp.Normalize();
	Vector3 unit(1.0f,0.0f,0.0f);

	float angleOfRotation = Math::Acos(Vector3::Dot(temp,unit));
	Vector3 axisOfRotation = Vector3::Cross(unit, temp);

	if (Math::NearZero(axisOfRotation.Length())) {
		mQuat = Quaternion::Identity;
		return;
	}

	axisOfRotation.Normalize();
	
	mPosition = mGame->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f));

	mQuat = Quaternion(axisOfRotation, angleOfRotation);

}
