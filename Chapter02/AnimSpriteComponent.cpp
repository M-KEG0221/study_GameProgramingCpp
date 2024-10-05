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
#include <iterator>

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
			mCurrFrame = static_cast<float>(currentAnim.last);
		}

		// Wrap current frame if needed
		//課題2
		while (mCurrFrame >= nextAnimStateHeadFrameIndex)
		{
			//課題2 描画対象のアニメーションの枚数分減算するようにする
			mCurrFrame -= currentAnim.CalcTotalFrames();
		}

		// Set the current texture
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);

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
	return mAnimations[mOwner->GetCurrAnimState()].last + 1;
}

//デフォルト引数はhファイルにのみ書く
//こっちにも書くと：コンパイルエラー（ https://qiita.com/yut-nagase/items/29d0fc0984e6dbace85e ）
//こっちにのみ書くと：構文エラー
//void AnimSpriteComponent::PushAnimTexture(const int totalFrame, const bool isLoop)
//{
//	int pushedAnims = mAnimations.size();
//
//	if (pushedAnims == 0)
//	{
//		mAnimations.push_back(Animation(0, totalFrame - 1, isLoop));
//	}
//	else
//	{
//		int prevAnimLastIndex = mAnimations[pushedAnims - 1].last;
//
//		mAnimations.push_back(Animation(
//			prevAnimLastIndex + 1,
//			prevAnimLastIndex + totalFrame,
//			isLoop
//		));
//	}
//}

//void AnimSpriteComponent::PushAnimTexture(const int head, const int tail, const bool isLoop)
//{
//	mAnimations.push_back(Animation(head, tail, isLoop));
//}

int AnimSpriteComponent::RegisterAnimation(const std::vector<SDL_Texture*>& textures, const bool isLoop)
{
	int pushedAnims = mAnimations.size();

	if (pushedAnims == 0)
	{
		mAnimations.push_back(Animation(0, textures.size() - 1, isLoop));
	}
	else
	{
		int prevAnimLastIndex = mAnimations[pushedAnims - 1].last;

		mAnimations.push_back(Animation(
			prevAnimLastIndex + 1,
			prevAnimLastIndex + textures.size(),
			isLoop
		));
	}

	std::vector<SDL_Texture*> result;
	result = mAnimTextures;

	result.reserve(textures.size()); // 事前にメモリを確保することで効率化
	std::copy(textures.begin(), textures.end(), std::back_inserter(result));
	SetAnimTextures(result);

	// 既に追加されてるアニメーション数 = 今回追加したアニメーションのインデックス
	return pushedAnims;
}