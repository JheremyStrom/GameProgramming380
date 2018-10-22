#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "CameraComponent.h"

#include <algorithm>

Actor::Actor(Game* game)
	:mGame(game)
	,mState(EActive)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
	,mMove(nullptr)
	,mCollision(nullptr)
	,mMesh(nullptr)
	,mCamera(nullptr)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	if (mMove != nullptr)
	{
		delete mMove;
	}
	if (mCollision != nullptr)
	{
		delete mCollision;
	}
	if (mMesh != nullptr)
	{
		delete mMesh;
	}
	if(mCamera != nullptr)
	{
		delete mCamera;
	}
	mGame->RemoveActor(this);
}

void Actor::Update(float deltaTime)
{
		if (mState == EActive)
		{
			// Update Components
			if (mMove != nullptr)
			{
				mMove->Update(deltaTime);
			}
			if (mCollision != nullptr)
			{
			}
			if (mMesh != nullptr)
			{
			}
			if (mCamera != nullptr) {
				mCamera->Update(deltaTime);
			}

			UpdateActor(deltaTime);
		}

		float scale[4][4] = {
			{ mScale , 0, 0, 0},
			{0, mScale , 0, 0},
			{0, 0, mScale, 0},
			{0, 0, 0, 1}
		};

		float transform[4][4] = {
			{ 1 , 0, 0, 0 },
			{ 0, 1 , 0, 0 },
			{ 0, 0, 1, 0 },
			{ mPosition.x, mPosition.y, mPosition.z, 1 }
		};

		float rotation[4][4] = {
			{ Math::Cos(mRotation) , Math::Sin(mRotation), 0, 0 },
			{ -Math::Sin(mRotation), Math::Cos(mRotation) , 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
		};

		mWorldTransform = Matrix4(scale) * Matrix4(rotation) * Matrix4::CreateFromQuaternion(mQuat) * Matrix4(transform);
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	if (mState == EActive)
	{
		// ProcessInput of components
		if (mMove != nullptr)
		{
			mMove->ProcessInput(keyState);
		}
		if (mCollision != nullptr)
		{
		}
		if (mMesh != nullptr)
		{
		}
		if (mCamera != nullptr) {
			mCamera->ProcessInput(keyState);
		}
		ActorInput(keyState);
	}
}

void Actor::ActorInput(const Uint8* keyState)
{
}

Vector3 Actor::GetForward() const {
	return Vector3(Math::Cos(mRotation), Math::Sin(mRotation), 0.0f);
}

Vector3 Actor::GetRight() const {
	return Vector3(Math::Cos(mRotation+Math::PiOver2), Math::Sin(mRotation+Math::PiOver2), 0.0f);
}

MoveComponent* Actor::GetMove() const { return mMove; }
void Actor::SetMove(MoveComponent* move) { mMove = move; }
CollisionComponent* Actor::GetCollision() const { return mCollision; }
void Actor::SetCollision(CollisionComponent* collision) { mCollision = collision; }
void Actor::SetQuat(Quaternion quat) { mQuat = quat; }