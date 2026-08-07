#pragma once
#include <Texture.h>
#include "Player.h"
#include "Enemy.h"
#include "Text.h"

class HUD
{
public:
	HUD();
	~HUD();
	void Draw(const Player& player, const Enemy& enemy) const;
	void Update(const Player& player, int score);

private:
	Texture* m_pHUD{};
	Texture* m_pLifeBar{};
	Text m_CraneKickText;
	int m_LastCraneKickCount{ -1 };
	Text m_DrumPunchText;
	int m_LastDrumPunchCount{ 2 };

	Text m_ScoreText;
	int m_LastScore{ -1 };
};