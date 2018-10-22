//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Block.h"
#include "LevelLoader.h"
#include "Checkpoint.h"
#include "Arrow.h"
#include <SDL/SDL_ttf.h>
#include "HUD.h"

Game::Game()
:mIsRunning(true), mWindowWidth(1024.0f), mWindowHeight(768.0f)
{
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(mWindowWidth, mWindowHeight)) {
		return false;
	}
	

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	TTF_Init();

	LoadData();

	mTicksCount = SDL_GetTicks();

	SDL_SetRelativeMouseMode(SDL_TRUE); // Enable relative mouse mode 
	SDL_GetRelativeMouseState(nullptr, nullptr); // Clear any saved values 
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
		if (mNextLevel.size() != 0) {
			LoadNextLevel();
		}
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
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
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
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Update the HUD
	mHUD->Update(deltaTime);

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete any of the dead actors (which will
	// remove them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	LoadSound("Assets/Sounds/Checkpoint.wav");
	LoadSound("Assets/Sounds/Coin.wav");
	LoadSound("Assets/Sounds/Jump.wav");
	LoadSound("Assets/Sounds/Land.wav");
	LoadSound("Assets/Sounds/Music.ogg");
	LoadSound("Assets/Sounds/Running.wav");

	//LoadLevel("Assets/Level.txt");
	LevelLoader::Load(this, "Assets/Tutorial.json");
	mRenderer->SetProjectionMatrix(Matrix4::CreatePerspectiveFOV(1.22f, mWindowWidth, mWindowHeight, 10.0f, 10000.0f));
	Vector3 target(5.0f, 5.0f, 5.0f);
	Vector3 up(0.0f, 0.0f, 1.0f);
	mRenderer->SetViewMatrix(Matrix4::CreateLookAt(Vector3::Zero, target, up));


	mArrow = new Arrow(this);
	mHUD = new HUD(this);
	Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
}

void Game::LoadLevel(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load level: %s", fileName.c_str());
	}

	const float topLeftX = -512.0f + 32.0f;
	const float topLeftY = -384.0f + 32.0f;
	size_t row = 0;
	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		for (size_t col = 0; col < line.size(); col++)
		{
			// Calculate position at this row/column
			Vector3 pos;
			pos.x = topLeftX + 64.0f * col;
			pos.y = topLeftY + 64.0f * row;

			if (line[col] == 'B')
			{
				Block* block = new Block(this);
				block->SetPosition(pos);
			}
			else if (line[col] == 'P')
			{
			}
			else if (line[col] == 'Q')
			{
			}
		}
		row++;
	}
}

void Game::LoadNextLevel() {
	// Kill all current actors
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Kill all current checkpoints
	while (!mCheckpoints.empty())
	{
		mCheckpoints.pop();
	}

	LevelLoader::Load(this, mNextLevel);

	mArrow = new Arrow(this);

	mNextLevel.clear();
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

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

void Game::LoadSound(const std::string& fileName)
{
	Mix_Chunk* chunk = Mix_LoadWAV(fileName.c_str());
	if (!chunk)
	{
		SDL_Log("Failed to load sound file %s", fileName.c_str());
		return;
	}

	mSounds.emplace(fileName, chunk);
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	mRenderer->Shutdown();
	delete mRenderer;
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}

void Game::AddBlock(Block* block)
{
	mBlocks.emplace_back(block);
}

void Game::RemoveBlock(Block* block)
{
	auto iter = std::find(mBlocks.begin(), mBlocks.end(), block);
	if (iter != mBlocks.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mBlocks.end() - 1;
		std::iter_swap(iter, iter2);
		mBlocks.pop_back();
	}
}

void Game::AddCheckpoint(Checkpoint* checkpoint) {
	mCheckpoints.push(checkpoint);
}

Checkpoint* Game::RemoveCheckpoint() {
	if (mCheckpoints.size() == 0) {
		return NULL;
	}

	Checkpoint* front = mCheckpoints.front();
	mCheckpoints.pop();
	return front;
}

Checkpoint* Game::FrontCheckpoint() {
	if (mCheckpoints.size() == 0) {
		return NULL;
	}
	else {
		return mCheckpoints.front();
	}
}

void Game::SetPlayer(class Player* player) {
	mPlayer = player;
}

Player* Game::GetPlayer() const {
	return mPlayer;
}