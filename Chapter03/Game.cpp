// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Random.h"
#include <iostream>

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
{

}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 3)", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

	Random::Init();

	LoadData();

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	OutputProblem1();
	OutputProblem2();
	OutputProblem3();

	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// Update all actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255);
	SDL_RenderClear(mRenderer);

	// Draw all sprite components
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}

	SDL_RenderPresent(mRenderer);
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetRotation(Math::PiOver2);

	// Create asteroids
	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(),
		mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Game::OutputProblem1()
{
	Vector2 a(2, 4);
	Vector2 b(3, 5);
	float s = 2;
	std::cout << "Åyâ€ëË3.1-1Åza=<2,4>, b=<3,5>, s=2" << std::endl;

	Vector2 ansA = a + b;// <2+3, 4+5> = <5,9>
	std::cout << "(a): a+b = " << "<" << ansA.x << "," << ansA.y << ">" << std::endl;

	Vector2 ansB = s * a;// <2*2, 2*4> = <4, 8>
	std::cout << "(b): s.a = " << "<" << ansB.x << "," << ansB.y << ">" << std::endl;

	float ansC = Vector2::Dot(a, b);//2*3+4*5 = 26
	std::cout << "(c): a.b = " << ansC << std::endl;

	std::cout << "----- " << std::endl;
}

void Game::OutputProblem2()
{
	Vector2 a(-1, 1);
	Vector2 b(2, 4);
	Vector2 c(3, 3);
	std::cout << "Åyâ€ëË3.1-2ÅzA=<-1,1>, B=<2,4>, C=<3, 3>" << std::endl;

	//ABÇãÅÇﬂÇÈ
	Vector2 ab(b - a);//<3, 3>
	Vector2 abN = Vector2::Normalize(ab);//<3, 3>

	//ACÇãÅÇﬂÇÈ
	Vector2 ac(c - a);//<4, 2>
	Vector2 acN = Vector2::Normalize(ac);//<4, 2>

	//AcosÇ…ABÇ∆ACÇÃê≥ãKâªÇµÇΩílÇÃÉhÉbÉgêœÇó^Ç¶ÇÈ
	float dot = Vector2::Dot(abN, acN);
	float angle = Math::Acos(dot);

	std::cout << "AB = " << "<" << ab.x << "," << ab.y << ">" << std::endl;
	std::cout << "AB^ = " << "<" << abN.x << "," << abN.y << ">" << std::endl;
	std::cout << "AC = " << "<" << ac.x << "," << ac.y << ">" << std::endl;
	std::cout << "AC^ = " << "<" << acN.x << "," << acN.y << ">" << std::endl;
	std::cout << "AB^.AC^ = " << dot << std::endl;
	std::cout << "É∆(ABÇ∆ACÇÃÇ»Ç∑äpÅj = " << angle << std::endl;
	std::cout << "É∆(ABÇ∆ACÇÃÇ»Ç∑äpÅj = " << Vector2::CalcAngle(ab, ac) << std::endl;//äpìxåvéZÇÃä÷êîâª
	std::cout << "----- " << std::endl;
}

void Game::OutputProblem3()
{
	Vector2 wp(1, 0);
	Vector2 player(4, 0);
	Vector2 nwp(5, 6);
	std::cout << "Åyâ€ëË3.1-3Åz" << std::endl;

	Vector2 nwpN = Vector2::Normalize(nwp);
	std::cout << "(a): newWaypointÇÃíPà ÉxÉNÉgÉã = " <<
		"<" << nwpN.x << "," << nwpN.y << ">"
		<< std::endl;

	Vector2 pToWp = wp - player;
	Vector2 pToNwp = nwp - player;
	std::cout << "(b): playerÇ©ÇÁÇ›ÇΩêVãåwaypointÇÃâÒì]äp = " << Vector2::CalcAngle(pToWp, pToNwp) << std::endl;

	Vector3 pToWp3(pToWp);
	Vector3 pToNwp3(pToNwp);
	Vector3 cross = Vector3::Cross(pToWp3, pToNwp3);
	std::cout << "(c): êVãåwaypointÇ©ÇÁÇ»ÇÈïΩñ Ç…íºçsÇ∑ÇÈÉxÉNÉgÉã " <<
		"<" << cross.x << "," << cross.y << "," << cross.z << ">"
		<< std::endl;
}