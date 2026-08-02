#include "pch.h"
#include "HUD.h"

HUD::HUD()
{
	m_pHUD = new Texture("HUD.png");
	m_pLifeBar = new Texture("Rectangle.png");
}

HUD::~HUD()
{
	delete m_pHUD;
	delete m_pLifeBar;
	m_pHUD = nullptr;
	m_pLifeBar = nullptr;
}

void HUD::Draw(const Player& player, const Enemy& enemy) const
{
	m_pHUD->Draw(Vector2f{0.f, 520.f});

	for (int i{}; i < player.GetHealth(); ++i)
	{
		Rectf drawRect{ 155.f + i * 10.5f, 605.f, 8.f, 32.f };
		m_pLifeBar->Draw(drawRect);
	}

	for (int i{}; i < enemy.GetHealth(); ++i)
	{
		Rectf drawRect{ 155.f + i * 10.5f, 547.f, 8.f, 32.f };
		m_pLifeBar->Draw(drawRect);
	}
}
