// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Actor.h"
class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;
	class CircleComponent* GetCircle() { return mCircle; }
private:
	enum class ShipState {
		Alive, Dead
	};
	ShipState mState;
	class SpriteComponent* mSc;

	float mLaserCooldown;
	class CircleComponent* mCircle;

	float mRespawnTime;
	float mElapsedDeadTime;
};