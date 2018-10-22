#include "PlayerMove.h"
#include "Actor.h"
#include "SDL/SDL.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Block.h"
#include "Player.h"

#include <string>
//SDL_Log(std::to_string(directionParallel).c_str()); float to c-string for SDL_Log()


PlayerMove::PlayerMove(Actor* owner) : MoveComponent(owner), mSpacePressed(false), mMass(1.0f),
mWallClimbTimer(0.0f), mWallRunTimer(0.0f) {
	mGravity = Vector3(0.0f, 0.0f, -980.0f);
	mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
	mWallForce = Vector3(0.0f, 0.0f, 1800.0f);
	mWallRunForce = Vector3(0.0f, 0.0f, 1200.0f);
	ChangeState(Falling);
	mRunningSFX = Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav"), -1);
	Mix_Pause(mRunningSFX);
}

PlayerMove::~PlayerMove()
{
	Mix_HaltChannel(mRunningSFX);
}

void PlayerMove::Update(float deltaTime) {
	// Call specific update based on the state of the player
	switch (mCurrentState) {
		case OnGround: {
			UpdateOnGround(deltaTime);
			break; }
		case Jump: {
			UpdateJump(deltaTime);
			break;
		}
		case Falling: {
			UpdateFalling(deltaTime);
			break;
		}
		case WallClimb: {
			UpdateWallClimb(deltaTime);
			break;
		}
		case WallRun: {
			UpdateWallRun(deltaTime);
			break;
		}
	}

	if (mOwner->GetPosition().z < -750.0f) {
		((Player*) mOwner)->Respawn();
		mVelocity = Vector3::Zero;
		mPendingForces = Vector3::Zero;
		ChangeState(Falling);
	}
	// Play running sound
	if ((mVelocity.Length() > 50.0f && mCurrentState == OnGround) || mCurrentState == WallClimb 
		|| mCurrentState == WallRun) {
		Mix_Resume(mRunningSFX);
	}
	else {
		Mix_Pause(mRunningSFX);
	}
}

void PlayerMove::ProcessInput(const Uint8* keystate) {

	// Front/Back
	if (keystate[SDL_SCANCODE_W] && !keystate[SDL_SCANCODE_S]) {
		AddForce(mOwner->GetForward() * 700.0f);
	}
	if (keystate[SDL_SCANCODE_S] && !keystate[SDL_SCANCODE_W]) {
		AddForce(mOwner->GetForward() * -700.0f);
	}

	// Left/Right
	if (keystate[SDL_SCANCODE_A] && !keystate[SDL_SCANCODE_D]) {
		AddForce(mOwner->GetRight() * -700.0f);
	}
	if (keystate[SDL_SCANCODE_D] && !keystate[SDL_SCANCODE_A]) {
		AddForce(mOwner->GetRight() * 700.0f);
	}
	
	// Get mouse movement since last fram and update pitch and yaw
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	SetAngularSpeed((x/500.0f) * Math::Pi * 10.0f);
	mOwner->GetCamera()->SetPitchSpeed((y/500.0f) * Math::Pi * 10.0f);

	//SDL_Log(std::to_string(mZSpeed).c_str());
	if (mSpacePressed == false && keystate[SDL_SCANCODE_SPACE] == true && mCurrentState == OnGround) {
		AddForce(mJumpForce);
		ChangeState(Jump);
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav"), 0);
	}

	mSpacePressed = keystate[SDL_SCANCODE_SPACE];
}

void PlayerMove::ChangeState(MoveState state) {
	mCurrentState = state;
}

void PlayerMove::UpdateOnGround(float deltaTime) {
	// Update game variables
	PhysicsUpdate(deltaTime);

	// Fix collisions
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlocks();
	bool inAir = true;
	for (unsigned int i = 0; i < blocks.size(); ++i) {
		CollSide side = FixCollision(mOwner->GetCollision(), blocks[i]->GetCollision());
		// Currently on a block
		if (side == Top) {
			inAir = false;
		}
		// Commented out because we no longer want the player to wall climb from the OnGround state
		else if (side != Top && side != Bottom && side != None) {  // Any of the sides
			if (CanWallClimb(side)) {
				mWallClimbTimer = 0.0f;
				mCurrentState = WallClimb;
				return;
			}
		}
	}
	if (inAir) {
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateJump(float deltaTime) {
	// Update game variables
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	// Fix collisions
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlocks();
	for (unsigned int i = 0; i < blocks.size(); ++i) {
		CollSide side = FixCollision(mOwner->GetCollision(), blocks[i]->GetCollision());
		if (side == Bottom) {  // Head hit block
			mVelocity.z = 0.0f;
		}
		else if (side != Top && side != Bottom && side != None) {  // Any of the sides
			if (CanWallClimb(side)) {
				mWallClimbTimer = 0.0f;
				mCurrentState = WallClimb;
				return;
			}
			else if(CanWallRun(side)){  // Check if can wall run
				mWallRunTimer = 0.0f;
				mCurrentState = WallRun;
				return;
			}
		}
	}
	if (mVelocity.z <= 0.0f) {  // Now falling
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateFalling(float deltaTime) {
	// Update game variables
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	// Fix Collisions
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlocks();
	for (unsigned int i = 0; i < blocks.size(); ++i) {
		// Landed on a block
		if (FixCollision(mOwner->GetCollision(), blocks[i]->GetCollision()) == Top) {
			mVelocity.z = 0.0f;
			ChangeState(OnGround);
			Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav"), 0);
		}
	}
}

void PlayerMove::UpdateWallClimb(float deltaTime) {
	// Update game variables
	AddForce(mGravity);
	mWallClimbTimer += deltaTime;
	if (mWallClimbTimer <= 0.4f) {
		AddForce(mWallForce);
	}
	PhysicsUpdate(deltaTime);

	// Fix collisions
	bool inAir = true;
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlocks();
	for (unsigned int i = 0; i < blocks.size(); ++i) {
		CollSide side = FixCollision(mOwner->GetCollision(), blocks[i]->GetCollision());
		// Collided with side
		if (side != Top && side != Bottom && side != None) {
			inAir = false;
		}
	}

	// Not climbing
	if (inAir || mVelocity.z <= 0.0f) {
		mVelocity.z = 0;
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateWallRun(float deltaTime) {
	// Update game variables
	AddForce(mGravity);
	mWallRunTimer += deltaTime;
	if (mWallRunTimer <= 0.4f) {
		AddForce(mWallForce);
	}
	PhysicsUpdate(deltaTime);

	// Fix collisions
	bool inAir = true;
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlocks();
	for (unsigned int i = 0; i < blocks.size(); ++i) {
		FixCollision(mOwner->GetCollision(), blocks[i]->GetCollision());
	}

	// Not climbing
	if (mVelocity.z <= 0.0f) {
		ChangeState(Falling);
	}
}

PlayerMove::CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* block) {

	if (block->Intersect(self))  // Hit a block
	{
		Vector3 currPos = mOwner->GetPosition();

		float dy1 = block->GetMin().y - self->GetMax().y;
		float dy2 = block->GetMax().y - self->GetMin().y;

		float dx1 = block->GetMin().x - self->GetMax().x;
		float dx2 = block->GetMax().x - self->GetMin().x;

		float dz1 = block->GetMin().z - self->GetMax().z;
		float dz2 = block->GetMax().z - self->GetMin().z;

		float minY;
		float minX;
		float minZ;
		bool top = false;
		CollSide ySide;
		CollSide xSide;
		// Determine mins of each axis
		if (Math::Abs(dy1) < Math::Abs(dy2)) {  // Which y is min
			minY = dy1;
			ySide = SideY1;
		}
		else { 
			minY = dy2; 
			ySide = SideY2;
		}
		if (Math::Abs(dx1) < Math::Abs(dx2)) {  // Which x is min
			minX = dx1;
			xSide = SideX1;
		}
		else { 
			minX = dx2; 
			xSide = SideX2;
		}
		if (Math::Abs(dz1) < Math::Abs(dz2)) {  // Which z is min
			minZ = dz1;
		}
		else { 
			minZ = dz2;
			top = true;
		}

		// Determine which side
		if (Math::Abs(minY) < Math::Abs(minX) && Math::Abs(minY) < Math::Abs(minZ)) {  // Close to a side
			currPos.y += minY;
			mOwner->SetPosition(currPos);

			// Normal force of wall
			if (ySide == SideY1) {
				AddForce(Vector3(0.0f, -700.0f, 0.0f));
			}
			else {
				AddForce(Vector3(0.0f, 700.0f, 0.0f));
			}

			return ySide;
		}
		else if(Math::Abs(minX) < Math::Abs(minY) && Math::Abs(minX) < Math::Abs(minZ)) {  // Close to a side
			currPos.x += minX;
			mOwner->SetPosition(currPos);

			// Normal force of wall
			if (xSide == SideX1) {
				AddForce(Vector3(-700.0f, 0.0f, 0.0f));
			}
			else {
				AddForce(Vector3(700.0f, 0.0f, 0.0f));
			}

			return xSide;
		}
		else {  // Closer to top/bottom
			currPos.z += minZ;
			mOwner->SetPosition(currPos);

			if (top) {
				return Top;
			}
			else {
				return Bottom;
			}
		}
	}
	else {
		return None;
	}
}

bool PlayerMove::CanWallClimb(CollSide cs) {
	// Check if the palyer is fast enough
	Vector2 xyDirection = Vector2(mVelocity.x, mVelocity.y);
	if (xyDirection.Length() <= 350.0f) {
		return false;
	}

	xyDirection.Normalize();

	// Get the normal of the block
	Vector2 blockNormal;
	switch (cs) {
		case SideX1: {
			blockNormal = Vector2(-1.0f, 0.0f);
			break;
		}
		case SideX2: {
			blockNormal = Vector2(1.0f, 0.0f);
			break;
		}
		case SideY1: {
			blockNormal = Vector2(0.0f, -1.0f);
			break;
		}
		case SideY2: {
			blockNormal = Vector2(0.0f, 1.0f);
			break;
		}
	}

	Vector2 forward2 = Vector2(mOwner->GetForward().x, mOwner->GetForward().y);

	// Check if player is "almost" facing the block
	float directionParallel = Vector2::Dot(forward2, blockNormal);
	float speedParallel = Vector2::Dot(xyDirection, blockNormal);
	if (directionParallel > -1.03f && directionParallel < -0.97f && 
		speedParallel > -1.01f && speedParallel < -0.99f) {  // around -1 because facing each other
		return true;
	}
	else return false;
}

bool PlayerMove::CanWallRun(CollSide cs) {
	// Check if the palyer is fast enough
	Vector2 xyDirection = Vector2(mVelocity.x, mVelocity.y);
	if (xyDirection.Length() <= 350.0f) {
		return false;
	}

	xyDirection.Normalize();

	// Get the normal of the block
	Vector2 blockNormal;
	switch (cs) {
		case SideX1: {
			blockNormal = Vector2(-1.0f, 0.0f);
			break;
		}
		case SideX2: {
			blockNormal = Vector2(1.0f, 0.0f);
			break;
		}
		case SideY1: {
			blockNormal = Vector2(0.0f, -1.0f);
			break;
		}
		case SideY2: {
			blockNormal = Vector2(0.0f, 1.0f);
			break;
		}
	}

	Vector2 forward2 = Vector2(mOwner->GetForward().x, mOwner->GetForward().y);

	// Check if player is "almost" facing the block
	float directionParallel = Vector2::Dot(forward2, blockNormal);
	float speedParallel = Vector2::Dot(xyDirection, forward2);
	if (directionParallel > -0.5f && directionParallel < 0.5f &&  // around 0 because tehy should be near perpendicular
		speedParallel < 1.01f && speedParallel > 0.99f) {  // around 1 because they should be in the same direction
		return true;
	}
	else return false;
}

void PlayerMove::PhysicsUpdate(float deltaTime) {
	// Update the physics
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += (mAcceleration * deltaTime);
	FixXYVelocity();

	// Update the player
	mOwner->SetPosition(mOwner->GetPosition() + mVelocity * deltaTime);
	mOwner->SetRotation(mOwner->GetRotation() + (GetAngularSpeed() * deltaTime));
	
	// Reset forces
	mPendingForces = Vector3::Zero;
}

void PlayerMove::AddForce(const Vector3& force) {
	mPendingForces += force;
}

void PlayerMove::FixXYVelocity() {
	Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
	// Cap the magnitude of the player's velocity
	if (xyVelocity.Length() > 400.0f) {
		xyVelocity = Vector2::Normalize(xyVelocity) * 400.0f;
	}

	// Apply braking
	if (mCurrentState == OnGround || mCurrentState == WallClimb) {
		if (Math::NearZero(mAcceleration.x) || (mAcceleration.x > 0 && xyVelocity.x < 0
			|| mAcceleration.x < 0 && xyVelocity.x > 0)) {  // Braking
			xyVelocity.x *= 0.9f;
		}
		if (Math::NearZero(mAcceleration.y) || (mAcceleration.y > 0 && xyVelocity.y < 0
			|| mAcceleration.y < 0 && xyVelocity.y > 0)) {  // Braking
			xyVelocity.y *= 0.9f;
		}
	}

	// Update velocity
	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}