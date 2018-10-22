#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Font.h"
#include <sstream>
#include <iomanip>

HUD::HUD(Game* game)
	:mGame(game)
	,mFont(nullptr)
	,mTimer(0.0f)
	,mCoinCount(0)
	,mMaxCoins(55)
	,mCheckpointTimer(0.0f)
{
	// Load font
	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");
	mTimerText = mFont->RenderText("00:00.00");
	mCoinText = mFont->RenderText("00/55");
	mCheckpointText = NULL;
}

HUD::~HUD()
{
	// Get rid of font
	if (mFont)
	{
		mFont->Unload();
		delete mFont;
	}
}

void HUD::Update(float deltaTime)
{
	mTimer += deltaTime;
	mTimerText->Unload();
	delete mTimerText;
	std::stringstream timer;
	timer << std::setfill('0') << std::setw(2) << ((int)mTimer)/60;
	timer << std::setw(1) << ":";
	timer << std::setfill('0') << std::setw(2) << ((int)mTimer) % 60;
	timer << std::setw(1) << ".";
	timer << std::setfill('0') << std::setw(2) << std::setprecision(0) << (int)((mTimer - ((int)mTimer)) * 100);
	mTimerText = mFont->RenderText(timer.str());

	// There currently is text being displayed
	if (mCheckpointText != NULL) {
		mCheckpointTimer += deltaTime;
		// 5 second display time
		if (mCheckpointTimer > 5.0f) {
			mCheckpointText->Unload();
			delete mCheckpointText;
			mCheckpointText = NULL;
		}
	}
	
}

void HUD::CollectedCoin() {
	++mCoinCount;
	mCoinText->Unload();
	delete mCoinText;
	std::stringstream coins;
	coins << std::setfill('0') << std::setw(2) << mCoinCount;
	coins << std::setw(1) << "/";
	coins << mMaxCoins;

	mCoinText = mFont->RenderText(coins.str());
}

void HUD::CheckpointHit(std::string text) {
	mCheckpointText = mFont->RenderText(text);
	mCheckpointTimer = 0.0f;
}

void HUD::Draw(Shader* shader)
{
	DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
	DrawTexture(shader, mCoinText, Vector2(-420.0f, -300.0f));
	if (mCheckpointText != NULL) {
		DrawTexture(shader, mCheckpointText, Vector2::Zero);
	}
}

void HUD::DrawTexture(class Shader* shader, class Texture* texture,
				 const Vector2& offset, float scale)
{
	// Scale the quad by the width/height of texture
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(texture->GetWidth()) * scale,
		static_cast<float>(texture->GetHeight()) * scale,
		1.0f);
	// Translate to position on screen
	Matrix4 transMat = Matrix4::CreateTranslation(
		Vector3(offset.x, offset.y, 0.0f));	
	// Set world transform
	Matrix4 world = scaleMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);
	// Set current texture
	texture->SetActive();
	// Draw quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
