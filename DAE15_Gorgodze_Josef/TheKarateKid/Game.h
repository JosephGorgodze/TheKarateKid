#pragma once
#include "BaseGame.h"
#include "Player.h"
#include "Enemy.h"
#include "HUD.h"
#include "Text.h"

enum class GameState
{
	Intro,
	Playing,
	RoundWon,
	RoundLost,
	GameOver
};

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	void UpdateGameplay(float elapsedSec);
	void UpdateIntro(float elapsedSec);
	void UpdateRoundWon(float elapsedSec);
	void UpdateRoundLost(float elaspedSec);
	void UpdateGameOver(float elapsedSec);
	// Variables
	Player m_Player;
	Enemy m_Enemy;
	Texture* m_pBackground{ nullptr };
	HUD m_HUD{};
	Text m_TitleText{ "PressStart2P-Regular.ttf", 32 };
	Text m_RoundText{ "PressStart2P-Regular.ttf", 32 };
	Text m_LivesText{ "PressStart2P-Regular.ttf", 32 };


	GameState m_GamesState{ GameState::Intro };

	int m_CurrentRound{ 1 };
	int m_Lives{ 3 };
	float m_StateTimer{};
};