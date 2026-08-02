#pragma once
#include <Texture.h>
#include "Player.h"
#include "Enemy.h"

class HUD
{
public:
	HUD();
	~HUD();
	void Draw(const Player& player, const Enemy& enemy) const;

private:
	Texture* m_pHUD{};
	Texture* m_pLifeBar{};
};