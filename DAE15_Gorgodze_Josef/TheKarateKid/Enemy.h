#pragma once
#include <Texture.h>

class Enemy
{
public:
	enum class State
	{
		Idle,
		Walk,
		Punch,
		Kick,
		Crouch,
		CrouchPunch,
		CrouchKick,
		Jump,
		Hit,
		Fall
	};

	Enemy();
	~Enemy();
	void Reset();
	void ResetHealth();
	void EnemyUpdate(float elapsedSec, const Rectf& playerBounds);
	void Draw() const;
	bool AttackIsActive() const;
	bool isAttacking() const;
	bool GetHasHit() const;
	void SetHasHit(bool hasHit);
	bool GetFacingRight() const;
	int GetComboHits() const;
	int GetMaxComboHits() const;
	Rectf GetBounds() const;
	Rectf GetAttackBox() const;
	Rectf GetHurtBox() const;

	int GetHealth() const;
	int GetMaxHealth() const;
	void TakeDamage(int damage);

	//Knockback
	void StartFall(bool hitFromRight);

private:
	Texture* m_pSpriteSheet{ nullptr };
	Rectf m_Bounds{ 500.f, 100.f, 80.f, 96.f };
	State m_State{ State::Idle };
	int m_FrameNr{};
	float m_AccuSec{};
	float m_FrameTime{ 0.15f };
	bool m_FacingRight{ false };
	float m_MoveSpeed{ 150.f };
	float m_MoveDirection{ -1.f };
	bool m_IsAttacking{ false };
	float m_TimeUntilAttack{};
	bool m_HasHit{};

	int m_Health{ 20 };
	static constexpr int m_MaxHealth{ 20 };

	//Knockback
	bool m_IsFalling{};
	float m_FallVelocityX{};
	float m_VelocityY{};
	const float m_Gravity{ -1000 };
	float m_GroundY{ 100 };

	//Combos
	int m_ComboHits{};
	const int m_MaxComboHits{3};

	Rectf GetCurrentFrame() const;
	void UpdateAnimation(float elapsedSec);
	int GetFrameCount() const;
	bool AnimationLoops() const;

	void Think(const Rectf& playerBounds);
	void Move(float elapsedSec);
	void StartAttack();
	void UpdateFall(float elapsedSec);
	State previousState{ m_State };
};

