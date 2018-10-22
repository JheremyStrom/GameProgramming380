#pragma once
#include <vector>
#include <SDL/SDL_stdinc.h>
#include "Math.h"
class Actor
{
public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};
	
	Actor(class Game* game);
	virtual ~Actor();

	// Update function called from Game (not overridable)
	void Update(float deltaTime);
	// Any actor-specific update code (overridable)
	virtual void UpdateActor(float deltaTime);
	// ProcessInput function called from Game (not overridable)
	void ProcessInput(const Uint8* keyState);
	// Any actor-specific update code (overridable)
	virtual void ActorInput(const Uint8* keyState);
	// Returns the forward direction vector of the actor
	Vector3 GetForward() const;
	// Returns the right direction vector of the actor
	Vector3 GetRight() const;

	// Getters/setters
	const Vector3& GetPosition() const { return mPosition; }
	void SetPosition(const Vector3& pos) { mPosition = pos; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale; }
	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; }
	class MoveComponent* GetMove() const;
	void SetMove(class MoveComponent*);
	class CollisionComponent* GetCollision() const;
	void SetCollision(class CollisionComponent*);
	class MeshComponent* GetMesh() const { return mMesh; }
	class CameraComponent* GetCamera() const { return mCamera; }
	void SetQuat(Quaternion);
	Quaternion GetQuat() const { return mQuat; }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	class Game* GetGame() { return mGame; }

protected:
	class Game* mGame;
	// Actor's state
	State mState;
	// Transform
	Vector3 mPosition;
	float mScale;
	float mRotation;
	Matrix4 mWorldTransform;
	Quaternion mQuat;

	// Components
	class MoveComponent* mMove;
	class CollisionComponent* mCollision;
	class MeshComponent* mMesh;
	class CameraComponent* mCamera;
};
