#include "pch.h"
#include "Game.h"
#include <ctime>
#include <cstdlib>
#include <utils.h>
#include <iostream>

Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup();
}

void Game::Initialize( )
{
	m_pBackground = new Texture("Level1.png");
	m_pHitMarker = new Texture("HitMarker.png");
	srand(unsigned(time(nullptr)));
}

void Game::Cleanup( )
{
	delete m_pBackground;
	m_pBackground = nullptr;
	delete m_pHitMarker;
	m_pHitMarker = nullptr;
}

void Game::Update( float elapsedSec )
{

	if (m_ShowHitMarker)
	{
		m_HitMarkerTimer -= elapsedSec;

		if (m_HitMarkerTimer <= 0.f)
		{
			m_ShowHitMarker = false;
		}
	}

	switch (m_GamesState)
	{
	case GameState::Intro:
		UpdateIntro(elapsedSec);
		break;
	case GameState::Playing:
		UpdateGameplay(elapsedSec);
		break;
	case GameState::FinishRound:
		UpdateFinishRound(elapsedSec);
		break;
	case GameState::RoundWon:
		UpdateRoundWon(elapsedSec);
		break;
	case GameState::RoundLost:
		UpdateRoundLost(elapsedSec);
		break;
	case GameState::GameOver:
		UpdateGameOver(elapsedSec);
		break;
	}
}

void Game::UpdateIntro(float elapsedSec)
{
	if (m_StateTimer <= 0.f)
	{
		if (m_CurrentRound == 1)
		{
			m_RoundText.SetText("FIRST ROUND");
		}
		else if (m_CurrentRound == 2)
		{
			m_RoundText.SetText("SECOND ROUND");
		}
		else if (m_CurrentRound == 3)
		{
			m_RoundText.SetText("FINAL ROUND");
		}
		m_TitleText.SetText("KARATE TOURNAMENT");
		m_LivesText.SetText("CHANCES REMAINING " + std::to_string(m_Lives) + ".");
	}
	m_StateTimer += elapsedSec;
	if (m_StateTimer >= 3.f)
	{
		m_StateTimer = 0.f;
		m_GamesState = GameState::Playing;
	}
}

void Game::UpdateGameplay(float elapsedSec)
{
	m_Player.PlayerUpdate(elapsedSec);
	m_Enemy.EnemyUpdate(elapsedSec, m_Player.GetBounds());
	bool playerIsRightOfEnemy = m_Player.GetBounds().left > m_Enemy.GetBounds().left;
	bool enemyIsRightOfPlayer = m_Enemy.GetBounds().left > m_Player.GetBounds().left;

	if (m_Player.AttackIsActive() && !m_Player.GetHasHit())
	{
		if (utils::IsOverlapping(m_Player.GetAttackBox(), m_Enemy.GetHurtBox()))
		{
			if (m_Enemy.GetComboHits() < m_Enemy.GetMaxComboHits())
			{
				std::cout << "Enemy hit!\n";
				m_Enemy.StartFall(playerIsRightOfEnemy);
				m_Player.SetHasHit(true);
				m_Enemy.TakeDamage(2);
				m_ShowHitMarker = true;
				m_HitMarkerTimer = 0.15f;
				Rectf hurtBox = m_Player.GetAttackBox();
				m_HitMarkerPos = Vector2f( hurtBox.left + hurtBox.width * 0.5f - 20.f, hurtBox.bottom + hurtBox.height * 0.5f - 20.f );
			}
		}
	}

	if (m_Enemy.AttackIsActive() && !m_Enemy.GetHasHit())
	{
		if (utils::IsOverlapping(m_Enemy.GetAttackBox(), m_Player.GetHurtBox()))
		{
			std::cout << "Player hit\n";
			m_Player.StartFall(enemyIsRightOfPlayer);
			m_Enemy.SetHasHit(true);
			m_Player.TakeDamage(2);
			m_ShowHitMarker = true;
			m_HitMarkerTimer = 0.15f;
			Rectf hurtBox = m_Enemy.GetAttackBox();
			m_HitMarkerPos = Vector2f( hurtBox.left + hurtBox.width * 0.5f - 20.f, hurtBox.bottom + hurtBox.height * 0.5f - 20.f );
		}
	}

	if (m_Enemy.GetHealth() <= 0)
	{
		m_Enemy.Defeat();
		m_PlayerWon = true;
		m_FinishTimer = 0.f;
		m_GamesState = GameState::FinishRound;
		m_StateTimer = 0.f;
		m_RoundText.SetText("ROUND WON");
	}

	if (m_Player.GetHealth() <= 0)
	{
		m_Player.Defeat();
		m_PlayerWon = false;
		m_GamesState = GameState::FinishRound;
		m_Lives--;
		std::cout << "Lives: " << m_Lives << '\n';
		m_LivesText.SetText("CHANCES REAMINING " + std::to_string(m_Lives) + ".");
		m_StateTimer = 0.f;
		m_RoundText.SetText("ROUND LOST");
	}
}

void Game::UpdateRoundWon(float elapsedSec)
{
	m_StateTimer += elapsedSec;

	if (m_StateTimer >= 3.f)
	{
		m_CurrentRound++;
		if (m_CurrentRound > 3)
		{
			m_RoundText.SetText("YOU WIN");
			m_GamesState = GameState::GameOver;
		}
		else
		{
			m_Enemy.Reset();
			m_Player.Reset();
			m_Enemy.ResetHealth();
			m_StateTimer = 0.f;
			m_GamesState = GameState::Intro;
		}
	}
}

void Game::UpdateRoundLost(float elapsedSec)
{
	m_StateTimer += elapsedSec;

	if (m_StateTimer >= 3.f)
	{

		if (m_Lives <= 0)
		{
			m_RoundText.SetText("GAME OVER");
			m_GamesState = GameState::GameOver;
		}
		else
		{
			m_Player.Reset();
			m_Enemy.Reset();
			m_Player.ResetHealth();
			m_LivesText.SetText("CHANCES REMAINING " + std::to_string(m_Lives) + ".");
			m_GamesState = GameState::Intro;
		}
		m_StateTimer = 0.f;
	}
}

void Game::UpdateGameOver(float elapsedSec)
{
	
}

void Game::UpdateFinishRound(float elapsedSec)
{
	m_FinishTimer += elapsedSec;

	if (m_FinishTimer >= 2.f)
	{
		if (m_PlayerWon)
		{
			m_RoundText.SetText("ROUND WON");
			m_GamesState = GameState::RoundWon;
		}
		else
		{
			m_RoundText.SetText("ROUND LOST");
			m_GamesState = GameState::RoundLost;
		}
		m_StateTimer = 0.f;
	}
}


void Game::Draw() const
{
	ClearBackground();


	switch (m_GamesState)
	{
	case GameState::Intro:
	case GameState::RoundLost:
	case GameState::RoundWon:
		m_TitleText.Draw(Vector2f(130.f, 430.f));
		m_RoundText.Draw(Vector2f{ 200.f, 300.f });
		m_LivesText.Draw(Vector2f{ 100.f, 120.f });
		break;
	case GameState::GameOver:
		m_RoundText.Draw(Vector2f(200.f, 300.f));
		break;
	case GameState::Playing:
	case GameState::FinishRound:
		m_pBackground->Draw();
		m_Player.Draw();
		m_Enemy.Draw();
		m_HUD.Draw(m_Player, m_Enemy);

		utils::SetColor(Color4f(1.f, 1.f, 1.f, 1.f));
		//utils::DrawRect(m_Player.GetAttackBox());
		//utils::DrawRect(m_Player.GetHurtBox());

		utils::SetColor(Color4f(1.f, 0.f, 0.f, 1.f));
		//utils::DrawRect(m_Enemy.GetAttackBox());
		//utils::DrawRect(m_Enemy.GetHurtBox());

		if (m_ShowHitMarker)
		{
			m_pHitMarker->Draw(m_HitMarkerPos);
		}
		break;
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}
