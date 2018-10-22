#pragma once
#include "Component.h"
class CameraComponent : public Component
{
public:
	CameraComponent(class Actor*);
	~CameraComponent();

	void Update(float deltaTime) override;

	// Setters/Getters
	float GetPitchSpeed() { return mPitchSpeed; }
	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }

private:
	float mPitchAngle;
	float mPitchSpeed;
};

