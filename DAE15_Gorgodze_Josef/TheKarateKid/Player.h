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
	void UpdateAnimation(float elapsedSec);
	bool AnimationLoops() const;
	Rectf GetBounds() const;

	bool IsAttacking() const;
	bool GetHasHit() const;
	void SetHasHit(bool hasHit);
	Rectf GetHurtBox() const;
	Rectf GetAttackBox() const;
private:
	void HandleMovement(float elapsedSec, const Uint8* keyboardState); //Detects state of every key on the keyboard

	Texture* m_pSpriteSheet{ nullptr };
	const float m_GroundY{ 100.f };
	Rectf m_Bounds{100.f, m_GroundY, 80.f, 96.f};
	State m_State{ State::Idle };
	int m_FrameNr{};
	float m_AccuSec{};
	float m_FrameTime{ 0.15f };
	bool m_FacingRight{ true };
	float m_VelocityY{};
	const float m_Gravity{-1000};
	bool m_OnGround{ true };
	bool m_IsAttacking{};
	bool m_IsCrouching{ false };
	bool m_HasHit{};

	Rectf GetCurrentFrame() const;
	int GetFrameCount() const;
	void StartAttack(State attackState);
};

