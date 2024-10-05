// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SpriteComponent.h"
#include <vector>

//template<typename TAnimState>
class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
	// Update animation every frame (overridden from component)
	void Update(float deltaTime) override;
	// Set the textures used for animation
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);

	// 課題2
	//void PushAnimTexture(const int totalFrame, const bool isLoop = true);
	//void PushAnimTexture(const int first, const int last, const bool isLoop = true);
	int RegisterAnimation(const std::vector<SDL_Texture*>& textures, const bool isLoop = true);

	// Set/get the animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }
private:
	//課題2 アニメーション情報
	struct Animation {
		Animation(const int head, const int tail, const bool isLoop = true)
			:first(head)
			, last(tail)
			, isLoop(isLoop) {}

		int first;
		int last;
		bool isLoop;

		int CalcTotalFrames() const { return last - first + 1; }
	};
	//int CalcNextAnimStateHeadFrameIndex();

	// All textures in the animation
	std::vector<SDL_Texture*> mAnimTextures;

	//課題2
	int mOldAnimState;					// 1F前のownerのanimStateを管理
	std::vector<Animation> mAnimations; // 各Animの最初と最後のframeのindexを管理する

	// Current frame displayed
	float mCurrFrame;
	// Animation frame rate
	float mAnimFPS;
};
