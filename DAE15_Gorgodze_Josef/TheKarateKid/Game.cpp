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
	m_pMainMenu = new Texture("MainMenu.png");

	srand(unsigned(time(nullptr)));

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	m_pMenuMusic = Mix_LoadMUS("MenuMusic.mp3");
	m_pFightMusic = Mix_LoadMUS("FightMusic.mp3");
	m_pPunchSound = Mix_LoadWAV("Punch.wav");
	m_pHitSound = Mix_LoadWAV("Hit.wav");
	m_pWinSound = Mix_LoadWAV("Win.wav");
	m_pLoseSound = Mix_LoadWAV("Lose.wav");

	if (!m_pMenuMusic)
	{
		std::cout << Mix_GetError() << '\n';
	}
	Mix_PlayMusic(m_pMenuMusic, -1);

	if (!m_pPunchSound)
	{
		std::cout << "Punch " << Mix_GetError() << '\n';
	}

	if (!m_pHitSound)
	{
		std::cout << "Hit " << Mix_GetError() << '\n';
	}
}

void Game::Cleanup( )
{
	delete m_pBackground;
	m_pBackground = nullptr;
	delete m_pHitMarker;
	m_pHitMarker = nullptr;
	delete m_pMainMenu;
	m_pMainMenu = nullptr;

	Mix_FreeMusic(m_pMenuMusic);
	m_pMenuMusic = nullptr;
	Mix_FreeMusic(m_pFightMusic);
	m_pFightMusic = nullptr;
	Mix_CloseAudio();
	Mix_FreeChunk(m_pPunchSound);
	m_pPunchSound = nullptr;
	Mix_FreeChunk(m_pHitSound);
	m_pHitSound = nullptr;
}

void Game::Update( float elapsedSec )
{

	m_HUD.Update(m_Player, m_Score);

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
	case GameState::MainMenu:
		UpdateMainMenu(elapsedSec);
		break;
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

		if (Mix_PlayingMusic())
		{
			Mix_HaltMusic();
		}
		Mix_PlayMusic(m_pFightMusic, -1);
	}
}

void Game::UpdateGameplay(float elapsedSec)
{
	m_Player.PlayerUpdate(elapsedSec);
	m_Enemy.EnemyUpdate(elapsedSec, m_Player.GetBounds());
	bool playerIsRightOfEnemy = m_Player.GetBounds().left > m_Enemy.GetBounds().left;
	bool enemyIsRightOfPlayer = m_Enemy.GetBounds().left > m_Player.GetBounds().left;

	if (m_Player.JustStartedAttack())
	{
		Mix_PlayChannel(-1, m_pPunchSound, 0);
	}

	if (m_Enemy.JustStartedAttack())
	{
		Mix_PlayChannel(-1, m_pPunchSound, 0);
	}

	if (m_Player.AttackIsActive() && m_Player.GetHitsThisAttack() < m_Player.GetMaxHitsThisAttack() && m_Player.CanHitFrame())
	{
		if (utils::IsOverlapping(m_Player.GetAttackBox(), m_Enemy.GetHurtBox()))
		{
			if (m_Enemy.GetComboHits() < m_Enemy.GetMaxComboHits())
			{
				std::cout << "Enemy hit!\n";
				if (m_Player.GetState() == Player::State::CraneKick)
				{
					if (m_Player.GetCurrentFrameNumber() == 2)
					{
						m_Enemy.StartHitStun();
					}
					else if (m_Player.GetCurrentFrameNumber() == 3)
					{
						m_Enemy.StartFall(playerIsRightOfEnemy);
					}
				}
				else if (m_Player.GetState() == Player::State::DrumPunch)
				{
					if (m_Player.GetFrameNr() == 2)
					{
						m_Enemy.StartHitStun();
					}
					else if (m_Player.GetFrameNr() == 4)
					{
						m_Enemy.StartFall(playerIsRightOfEnemy);
					}
				}
				else
				{
					m_Enemy.StartFall(playerIsRightOfEnemy);
				}
				Mix_PlayChannel(-1, m_pHitSound, 0);
				m_Player.AddHitThisAttack();
				m_Player.RegisterHitFrame();
				m_Enemy.TakeDamage(2);
				m_Score += 100;
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
			if (m_Player.GetComboHits() < m_Player.GetMaxComboHits())
			{
				std::cout << "Player hit\n";
				Mix_PlayChannel(-1, m_pHitSound, 0);
				m_Enemy.SetHasHit(true);
				m_Player.TakeDamage(2);
				m_ShowHitMarker = true;
				m_HitMarkerTimer = 0.15f;
				Rectf hurtBox = m_Enemy.GetAttackBox();
				m_HitMarkerPos = Vector2f(hurtBox.left + hurtBox.width * 0.5f - 20.f, hurtBox.bottom + hurtBox.height * 0.5f - 20.f);

				if (m_Player.GetHealth() <= 0)
				{
					Mix_HaltMusic();
					Mix_PlayChannel(-1, m_pLoseSound, 0);
					m_Player.Defeat();
					m_PlayerWon = false;
					m_FinishTimer = 0.f;
					m_GamesState = GameState::FinishRound;
					m_Lives--;
					std::cout << "Lives: " << m_Lives << '\n';
					m_LivesText.SetText("CHANCES REMAINING " + std::to_string(m_Lives) + '.');
					m_StateTimer = 0.f;
					m_Score -= 500;
					if (m_Score < 0)
					{
						m_Score = 0;
					}
				}
				else
				{
					m_Player.StartFall(enemyIsRightOfPlayer);
				}
			}
		}
	}

	if (m_Enemy.GetHealth() <= 0)
	{
		Mix_HaltMusic();
		Mix_PlayChannel(-1, m_pWinSound, 0);

		m_Enemy.Defeat();
		m_PlayerWon = true;
		m_FinishTimer = 0.f;
		m_GamesState = GameState::FinishRound;
		m_StateTimer = 0.f;
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
	if (m_PlayerWon)
	{
		return;
	}

	m_StateTimer += elapsedSec;

	if (m_StateTimer >= 3.f)
	{
		m_StateTimer = 0.f;
		m_CurrentRound = 1;
		m_Lives = 3;
		m_Player.Reset();
		m_Player.ResetHealth();
		m_Enemy.Reset();
		m_Enemy.ResetHealth();
		m_PlayerWon = false;
		m_LivesText.SetText("CHANCES REMAINING " + std::to_string(m_Lives) + ".");
		Mix_HaltMusic();
		Mix_PlayMusic(m_pMenuMusic, -1);
		m_GamesState = GameState::MainMenu;
		m_Score = 0;
	}
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

void Game::UpdateMainMenu(float elapsedSec)
{

}


void Game::Draw() const
{
	ClearBackground();


	switch (m_GamesState)
	{
	case GameState::MainMenu:
		m_pMainMenu->Draw();
		break;
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
	if (m_GamesState == GameState::MainMenu)
	{
		if (e.keysym.sym == SDLK_RETURN || e.keysym.sym == SDLK_KP_ENTER)
		{
			Mix_HaltMusic();
			m_GamesState = GameState::Intro;
			m_StateTimer = 0.f;
		}
	}

	if (e.keysym.sym == SDLK_p)
	{
		Mix_PlayChannel(-1, m_pPunchSound, 0);
	}
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
