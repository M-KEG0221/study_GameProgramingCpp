// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mPaddleDir(0)
	, mBalls({})
{

}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;

	//note 課題1 パドル2の初期値定義
	mPaddlePos2.x = 1024.0f - 10.0f - thickness;
	mPaddlePos2.y = 768.0f / 2.0f;

	//note 課題2 ボール複数の初期値定義
	Ball ball1;
	ball1.position.x = 1024.0f / 2.0f;
	ball1.position.y = 768.0f / 2.0f;
	ball1.velocity.x = -200.0f;
	ball1.velocity.y = 235.0f;
	Ball ball2;
	ball2.position.x = 1024.0f / 2.0f;
	ball2.position.y = 768.0f / 2.0f;
	ball2.velocity.x = 130.0f;
	ball2.velocity.y = -335.0f;
	mBalls = { ball1, ball2 };

	return true;
}

void Game::RunLoop()
{
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
			// If we get an SDL_QUIT event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Update paddle direction based on W/S keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

	mPaddleDir2 = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddleDir2 -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddleDir2 += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update paddle position based on direction
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	//note: 課題2 パドル2の更新
	if (mPaddleDir2 != 0)
	{
		mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos2.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos2.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos2.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos2.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	// Update ball position based on ball velocity
	for (Ball& ball : mBalls)
	{
		ball.position.x += ball.velocity.x * deltaTime;
		ball.position.y += ball.velocity.y * deltaTime;

		// Bounce if needed
		// Did we intersect with the paddle?
		float diff = mPaddlePos.y - ball.position.y;
		float diff2 = mPaddlePos2.y - ball.position.y;
		// Take absolute value of difference
		diff = (diff > 0.0f) ? diff : -diff;
		diff2 = (diff2 > 0.0f) ? diff2 : -diff2;
		if (
			(//左パドル
				// Our y-difference is small enough
				diff <= paddleH / 2.0f &&
				// We are in the correct x-position
				ball.position.x <= 25.0f && ball.position.x >= 20.0f && //25は左paddleの右辺, 20は単なる有効範囲？
				// The ball is moving to the left
				ball.velocity.x < 0.0f)
			||
			(//右パドル
				// Our y-difference is small enough
				diff2 <= paddleH / 2.0f &&
				// We are in the correct x-position
				ball.position.x <= mPaddlePos2.x + 5 && ball.position.x >= mPaddlePos2.x && // mPaddlePos2.x + 5は有効範囲、 mPaddlePos2.xは右paddleの左辺
				// The ball is moving to the left
				ball.velocity.x > 0.0f)
			)
		{
			ball.velocity.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (ball.position.x <= 0.0f)
		{
			mIsRunning = false;
		}
		// Did the ball collide with the right wall?
		else if (ball.position.x >= (1024.0f - thickness) && ball.position.x > 0.0f)
		{
			mIsRunning = false;
		}

		// Did the ball collide with the top wall?
		if (ball.position.y <= thickness && ball.velocity.y < 0.0f)
		{
			ball.velocity.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (ball.position.y >= (768 - thickness) &&
			ball.velocity.y > 0.0f)
		{
			ball.velocity.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);

	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);


	// Draw paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	//note: 課題2　パドル2を追加
	SDL_Rect paddle2 = paddle;
	paddle2.x = mPaddlePos2.x;
	paddle2.y = mPaddlePos2.y;
	SDL_RenderFillRect(mRenderer, &paddle2);


	// Draw ball
	for (const Ball& const ball : mBalls)
	{
		SDL_Rect b{
			static_cast<int>(ball.position.x - thickness / 2),
			static_cast<int>(ball.position.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &b);
	}

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
