#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
:Component(owner)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
,mStrafeSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	mOwner->SetRotation(mOwner->GetRotation() + mAngularSpeed * deltaTime);

	Vector3 temp = (mOwner->GetForward() * mForwardSpeed * deltaTime);
	temp += mOwner->GetPosition();	
	//mOwner->SetPosition(temp);

	temp += mOwner->GetRight() * mStrafeSpeed * deltaTime;
	mOwner->SetPosition(temp);
}
