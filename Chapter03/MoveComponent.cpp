// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponent.h"
#include "Actor.h"
#include <iostream>

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	, mAngularSpeed(0.0f)
	, mForwardSpeed(0.0f)
	, mForce(Vector2())
	, mFriction(0.0f)
{

}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);

		std::cout << rot << std::endl;
	}

	float registSpeed = -mForwardSpeed;

	//if (!Math::NearZero(mForwardSpeed)) // mForward依存だと、No Input状態で速度が変更されないため変更する
		//if (!Math::NearZero(mVerocity.Length()))
		//if (
		//	!(Math::NearZero(mVerocity.x)
		//		&& Math::NearZero(mVerocity.y)
		//		&& Math::NearZero(mForce.Length())
		//		&& Math::NearZero(mForwardSpeed)
		//		//&& !Math::NearZero(mForce.LengthSq())
		//		))
	//{
		//}
		//F=m.aからa=F/mをし、加速度を求める
	Vector2 acceleration = (mForce - mVerocity * mFriction) / mMass;
	/*std::cout << mForce.x << std::endl;
	std::cout << (mOwner->GetForward() * mForwardSpeed).x << std::endl;
	std::cout << "-----" << std::endl;*/

	//v=v0+atから現在の速度にa*deltatimeを加算
	mVerocity += acceleration * deltaTime;

	/*std::cout << "limit speed" << std::endl;
	std::cout << mVerocity.LengthSq() << std::endl;*/
	//mVerocity.Normalize();
	//mVerocity *= mForwardSpeed;

	/*if (Vector2::CalcAngle(mOwner->GetForward(), mVerocity) == Math::Pi)
	{
		mVerocity = Vector2();
	}*/

	//x=x0+v0tから現在の位置にv*deltatimeを加算
	Vector2 pos = mOwner->GetPosition();
	//mVerocity = mOwner->GetForward() * mForwardSpeed;

	//pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
	pos += mVerocity * deltaTime;//deltaTimeが2回掛かってるから極端に遅くなる

	// (Screen wrapping code only for asteroids)
	if (pos.x < 0.0f) { pos.x = 1022.0f; }
	else if (pos.x > 1024.0f) { pos.x = 2.0f; }

	if (pos.y < 0.0f) { pos.y = 766.0f; }
	else if (pos.y > 768.0f) { pos.y = 2.0f; }

	mOwner->SetPosition(pos);

	if (mForce.Length() > 0)
	{
		//std::cout << mVerocity.x << std::endl;
		//std::cout << acceleration.x << std::endl;
		//std::cout << "----" << std::endl;
	}
	mForce = Vector2();
	//}
}

void MoveComponent::AddForce(Vector2 force)
{
	float deltaTime = 1.0f;

	std::cout << "追加: " << force.x << "," << force.y << std::endl;

	mForce += force;
	std::cout << "撃力: " << mForce.x << "," << mForce.y << std::endl;

	Vector2 a(mForce / mMass);
	std::cout << "加速度: " << a.x << "," << a.y << std::endl;
	Vector2 vBeforeN(mOwner->GetForward() * mForwardSpeed * deltaTime);
	Vector2 vAfterN(a * deltaTime);
	std::cout << "ニュートン前速度: " << vBeforeN.x << "," << vBeforeN.y << std::endl;
	std::cout << "ニュートン後速度: " << vAfterN.x << "," << vAfterN.y << std::endl;
	std::cout << "空気抵抗: " << vAfterN.x * mFriction << "," << vAfterN.y * mFriction << std::endl;
}

void MoveComponent::AddForce(float force)
{
	AddForce(mOwner->GetForward() * force);
}

void MoveComponent::AddForce()
{
	AddForce(mForwardSpeed * 60);
}
