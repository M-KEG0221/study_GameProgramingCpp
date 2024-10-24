// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Asteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Random.h"
#include "CircleComponent.h"
#include <iostream>

Asteroid::Asteroid(Game* game)
	:Actor(game)
	, mCircle(nullptr)
{
	// Initialize to random position/orientation
	Vector2 randPos = Random::GetVector(Vector2::Zero,
		Vector2(1024.0f, 768.0f));
	SetPosition(randPos);

	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	// Create a move component, and set a forward speed
	//MoveComponent* mc = new MoveComponent(this);
	//mc->SetForwardSpeed(150.0f);
	mMove = new MoveComponent(this);
	mMove->SetForwardSpeed(150.0f);

	// Create a circle component (for collision)
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(40.0f);

	//deltaTImeが追加で1回乗算される分、1/60秒へかける力を強化する
	mMove->AddForce(150.0f * 60);

	// Add to mAsteroids in game
	game->AddAsteroid(this);

}

void Asteroid::UpdateActor(float deltaTime)
{
	//mMove->AddForce(GetForward() * 150.0f);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}
