#pragma once
#include <Texture.h>
class Player
{
public:
	enum class State
	{
		Idle,
		Walk,
		Jump,
		Crouch,
		Punch,
		Kick,
		CrouchPunch,
		CrouchKick,
		Hit,
		Fall
	};
	Player();
	~Player();
	void PlayerUpdate(float elapsedSec);
	void Draw() const;
private:
	void HandleMovement(float elapsedSec, const Uint8* keyboardState); //Detects state of every key on the keyboard

	Texture* m_pSpriteSheet{ nullptr };
	Rectf m_Bounds{100.f, 100.f, 80.f, 96.f};
	State m_State{ State::Idle };
	int m_FrameNr{};
	float m_AccuSec{};
	float m_FrameTime{ 0.15f };
	bool m_FacingRight{ true };

	Rectf GetCurrentFrame() const;
};

