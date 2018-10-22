#pragma once
#include "Actor.h"
class Checkpoint : public Actor
{
public:
	Checkpoint(class Game*);
	~Checkpoint();

	void SetState(bool state);

	void UpdateActor(float deltaTime) override;

	void SetLevelString(std::string& levelString) { mLevelString = levelString; }
	void SetCheckpointString(std::string& checkpointText) { mCheckpointText = checkpointText; }

private:
	bool active;
	std::string mLevelString;
	std::string mCheckpointText;

};

