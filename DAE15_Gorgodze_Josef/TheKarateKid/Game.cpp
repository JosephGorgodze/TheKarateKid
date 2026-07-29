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
	srand(unsigned(time(nullptr)));
}

void Game::Cleanup( )
{
	delete m_pBackground;
	m_pBackground = nullptr;
}

void Game::Update( float elapsedSec )
{
	m_Player.PlayerUpdate(elapsedSec);
	m_Enemy.EnemyUpdate(elapsedSec, m_Player.GetBounds());
	//Hit detection
	if (m_Player.IsAttacking() && !m_Player.GetHasHit())
	{
		if (utils::IsOverlapping(m_Player.GetBounds(), m_Enemy.GetBounds()))
		{
			std::cout << "Enemy hit!\n";
			m_Player.SetHasHit(true);
		}
	}
		if (m_Enemy.isAttacking() && !m_Enemy.GetHasHit())
		{
			if (utils::IsOverlapping(m_Player.GetBounds(), m_Enemy.GetBounds()))
			{
				std::cout << "Player hit!\n";
				m_Enemy.SetHasHit(true);
			}
		}
}

void Game::Draw() const
{
	ClearBackground();

	m_pBackground->Draw();
	m_Player.Draw();
	m_Enemy.Draw();
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
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}
