#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include <queue>

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	// Player functions
	void SetPlayer(class Player* player);
	class Player* GetPlayer() const;

	// Block functions
	void AddBlock(class Block* block);
	void RemoveBlock(class Block* block);
	std::vector<class Block*> GetBlocks() { return mBlocks; }

	// Checkpoint queue functions
	void AddCheckpoint(class Checkpoint*);
	class Checkpoint* RemoveCheckpoint();
	class Checkpoint* FrontCheckpoint();

	void LoadSound(const std::string& fileName);
	Mix_Chunk* GetSound(const std::string& fileName);

	void LoadLevel(const std::string& fileName);

	class Renderer* GetRenderer() {	return mRenderer; }

	// Level related functions
	void SetNextLevel(std::string& nextLevel) { mNextLevel = nextLevel; }
	void LoadNextLevel();

	// HUD Functoins
	class HUD* GetHUD() { return mHUD; }

private:
	class Player* mPlayer;
	class Arrow* mArrow;
	class HUD* mHUD; 
	std::string mNextLevel;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// Map of textures loaded
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// Containers for game objects
	std::vector<class Actor*> mActors;
	std::vector<class Block*> mBlocks;
	std::queue<class Checkpoint*> mCheckpoints;

	class Renderer* mRenderer;

	Uint32 mTicksCount;
	bool mIsRunning;

	// Window variables
	const float mWindowWidth;
	const float mWindowHeight;
};
