#pragma once
#include "BaseGame.h"
#include "Player.h"
#include "Enemy.h"
#include "HUD.h"

enum class GameState
{
	MainMenu,
	Intro,
	Playing,
	FinishRound,
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
	void UpdateFinishRound(float elapsedSec);
	void UpdateMainMenu(float elapsedSec);
	// Variables
	Player m_Player;
	Enemy m_Enemy;
	Texture* m_pBackground{ nullptr };
	Texture* m_pHitMarker{ nullptr };
	Texture* m_pMainMenu{ nullptr };
	HUD m_HUD{};
	Text m_TitleText{ "PressStart2P-Regular.ttf", 32 };
	Text m_RoundText{ "PressStart2P-Regular.ttf", 32 };
	Text m_LivesText{ "PressStart2P-Regular.ttf", 32 };


	GameState m_GamesState{ GameState::MainMenu };

	int m_CurrentRound{ 1 };
	int m_Lives{ 3 };
	float m_StateTimer{};

	float m_FinishTimer{};
	bool m_PlayerWon{};

	bool m_ShowHitMarker{};
	float m_HitMarkerTimer{ 0.f };
	Vector2f m_HitMarkerPos{};

	Mix_Music* m_pMenuMusic{};
	Mix_Music* m_pFightMusic{};
	Mix_Chunk* m_pPunchSound{};
	Mix_Chunk* m_pHitSound{};
	Mix_Chunk* m_pWinSound{};
	Mix_Chunk* m_pLoseSound{};

	int m_Score{ 0 };
};