#include "pch.h"
#include "HUD.h"

HUD::HUD()
	: m_CraneKickText("PressStart2P-Regular.ttf", 30)
	, m_DrumPunchText("PressStart2P-Regular.ttf", 30)
	, m_ScoreText("PressStart2P-Regular.ttf", 30)
{
	m_pHUD = new Texture("HUD.png");
	m_pLifeBar = new Texture("Rectangle.png");
	m_CraneKickText.SetText("2");
	m_DrumPunchText.SetText("2");
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
	m_CraneKickText.Draw(Vector2f( 520.f, 550.f ));
	m_DrumPunchText.Draw(Vector2f(670.f, 550.f));
	m_ScoreText.Draw(Vector2f(560.f, 605.f));
}

void HUD::Update(const Player& player, int score)
{
	if (player.GetCraneKickCount() != m_LastCraneKickCount)
	{
		m_LastCraneKickCount = player.GetCraneKickCount();
		m_CraneKickText.SetText(std::to_string(m_LastCraneKickCount));
	}
	if (player.GetDrumPunchCount() != m_LastDrumPunchCount)
	{
		m_LastDrumPunchCount = player.GetDrumPunchCount();
		m_DrumPunchText.SetText(std::to_string(m_LastDrumPunchCount));
	}

	if (score != m_LastScore)
	{
		m_LastScore = score;
		m_ScoreText.SetText(std::to_string(score));
	}
}
