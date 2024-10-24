// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
public:
	// Lower update order to update first
	MoveComponent(class Actor* owner, int updateOrder = 10);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }

	void AddForce(Vector2 force);
	void AddForce(float force);
	void AddForce();
	void SetFriction(float friction) { mFriction = friction; }
	void ResetVelocity() { mVerocity = Vector2(); }
	//void AddForce(float force) { mForce += force; }
private:
	// Controls rotation (radians/second)
	float mAngularSpeed;
	// Controls forward movement (units/second)
	float mForwardSpeed;

	Vector2 mForce;
	//float mForce;
	float mMass = 1.0f;//0èúéZâÒî
	//float mVerocity;
	Vector2 mVerocity;

	float mFriction = 0;
};
