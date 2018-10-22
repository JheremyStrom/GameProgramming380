#pragma once
#include "MoveComponent.h"
#include "Math.h"
class PlayerMove :
	public MoveComponent
{
public:
	enum MoveState
	{
		OnGround, 
		Jump, 
		Falling,
		WallClimb,
		WallRun
	};

	enum CollSide
	{
		None, 
		Top, 
		Bottom, 
		SideX1,
		SideX2,
		SideY1,
		SideY2
	};

	PlayerMove(class Actor*);
	~PlayerMove();

	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keystate) override;

	void ChangeState(MoveState state);

	// Physics mechanics
	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);
	void FixXYVelocity();

	// Climbing mechanics
	bool CanWallClimb(CollSide cs);
	bool CanWallRun(CollSide cs);


protected:
	// Updates based on what state the player is in
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deletaTime);
	void UpdateFalling(float deltaTime);
	void UpdateWallClimb(float deltaTime);
	void UpdateWallRun(float deltaTime);

	CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);

private:
	MoveState mCurrentState;
	bool mSpacePressed;

	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass;
	Vector3 mGravity;
	Vector3 mJumpForce;
	Vector3 mWallForce;
	Vector3 mWallRunForce;

	float mWallClimbTimer;
	float mWallRunTimer;

	int mRunningSFX;
};

