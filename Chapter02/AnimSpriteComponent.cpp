// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AnimSpriteComponent.h"
#include "Math.h"
#include "Actor.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
	, mCurrFrame(0.0f)
	, mAnimFPS(24.0f)
	, mOldAnimState(0)
{
}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (mAnimTextures.size() > 0)
	{
		// Update the current frame based on frame rate
		// and delta time
		mCurrFrame += mAnimFPS * deltaTime;

		//課題2 mAnimTexturesにおける描画対象のアニメーションの最後のFrameのindexを取得
		int nextAnimStateHeadFrameIndex = CalcNextAnimStateHeadFrameIndex();
		int currAnimState = mOwner->GetCurrAnimState();
		Animation currentAnim = mAnimations[currAnimState];

		//animが切り替わった場合、描画するtextureをリセット
		if (mOldAnimState != currAnimState)
		{
			mCurrFrame = static_cast<float>(currentAnim.first);

			//printf("change: %d\n", static_cast<int>(mCurrFrame));
		}

		//ループしないアニメーションの場合、ラップで調整されるされる前に最後のframeを描画するようにする
		if (!currentAnim.isLoop && mCurrFrame > currentAnim.last)
		{
			//まだ完璧じゃない（ごくたまにフレームがリセットされる
			mCurrFrame = currentAnim.last;
		}

		// Wrap current frame if needed
		//課題2
		while (mCurrFrame >= nextAnimStateHeadFrameIndex)
		{
			//if (mCurrFrame < 22 * 2)printf("before Wrap: %f\n", mCurrFrame);

			//課題2 描画対象のアニメーションの枚数分減算するようにする
			mCurrFrame -= currentAnim.CalcAnimFrames();

			//if (mCurrFrame < 22 * 2)printf("after Wrap: %f, %d\n", mCurrFrame, currentAnim.CalcAnimFrames());
		}

		// Set the current texture
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
		//printf("draw: %d\n", static_cast<int>(mCurrFrame));
		mOldAnimState = currAnimState;
	}
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures)
{
	mAnimTextures = textures;
	if (mAnimTextures.size() > 0)
	{
		// Set the active texture to first frame
		mCurrFrame = 0.0f;
		SetTexture(mAnimTextures[0]);
	}
}

//課題2
int AnimSpriteComponent::CalcNextAnimStateHeadFrameIndex()
{
	//begin同士だと0になるため、animState+1で集計できるようにする
	//return std::reduce(std::begin(mSumEachAnimFrames), std::next(std::begin(mSumEachAnimFrames), mOwner->GetCurrAnimState() + 1));

	return mAnimations[mOwner->GetCurrAnimState()].last + 1;
}

//デフォルト引数はhファイルにのみ書く
//こっちにも書くと：コンパイルエラー（ https://qiita.com/yut-nagase/items/29d0fc0984e6dbace85e ）
//こっちにのみ書くと：構文エラー
void AnimSpriteComponent::PushAnimTexture(const int totalFrame, const bool isLoop)
{
	int pushedAnims = mAnimations.size();

	if (pushedAnims == 0)
	{
		mAnimations.push_back(Animation(0, totalFrame - 1, isLoop));
		//printf("%d| %d, %d\n", totalFrame, totalFrame - 1, isLoop);
	}
	else
	{
		int prevAnimLastIndex = mAnimations[pushedAnims - 1].last;

		mAnimations.push_back(Animation(
			prevAnimLastIndex + 1,
			prevAnimLastIndex + totalFrame,
			isLoop
		));
		//printf("%d| %d, %d, %d\n", totalFrame, prevAnimLastIndex + 1, prevAnimLastIndex + totalFrame, isLoop);
	}
	//printf("------------\n");
}

void AnimSpriteComponent::PushAnimTexture(const int head, const int tail, const bool isLoop)
{
	mAnimations.push_back(Animation(head, tail, isLoop));
}