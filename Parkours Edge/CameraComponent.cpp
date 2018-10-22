#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"


CameraComponent::CameraComponent(Actor* owner) :
	Component(owner)
	,mPitchAngle(0.0f)
	,mPitchSpeed(0.0f)
{
}


CameraComponent::~CameraComponent()
{
}

void CameraComponent::Update(float deltaTime) {
	// Pitch
	mPitchAngle += mPitchSpeed * deltaTime;
	mPitchAngle = Math::Clamp(mPitchAngle, -Math::PiOver2 / 2.0f, Math::PiOver2 / 2.0f);

	Matrix4 rotatePitch = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 rotateYaw = Matrix4::CreateRotationZ(mOwner->GetRotation());
	Matrix4 rotateCombo = rotatePitch * rotateYaw;

	Vector3 target = (mOwner->GetPosition() + 5.0f * Vector3::Transform(Vector3(1.0f, 0.0f, 0.0f), rotateCombo));
	Vector3 up(0.0f, 0.0f, 1.0f);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(Matrix4::CreateLookAt(mOwner->GetPosition(), target, up));
}
