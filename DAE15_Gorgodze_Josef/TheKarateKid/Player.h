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
	void Reset();
	void ResetHealth();
	void PlayerUpdate(float elapsedSec);
	void Draw() const;
	void UpdateAnimation(float elapsedSec);
	bool AnimationLoops() const;
	Rectf GetBounds() const;

	bool GetIsAttacking() const;
	bool AttackIsActive() const;
	bool GetHasHit() const;
	bool GetFacingRight() const;
	void SetHasHit(bool hasHit);
	void StartFall(bool hitFromRight);
	void Defeat();
	Rectf GetHurtBox() const;
	Rectf GetAttackBox() const;

	int GetHealth() const;
	int GetMaxHealth() const;
	void TakeDamage(int damage);
private:
	void HandleMovement(float elapsedSec, const Uint8* keyboardState); //Detects state of every key on the keyboard

	Texture* m_pSpriteSheet{ nullptr };
	float m_MoveSpeed{ 300.f };
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

	int m_Health{20};
	static constexpr int m_MaxHealth{ 20 };

	//Knockback
	bool m_IsFalling{};
	float m_FallVelocityX{};

	Rectf GetCurrentFrame() const;
	int GetFrameCount() const;
	void StartAttack(State attackState);
	void UpdateFall(float elapsedSec);
};

