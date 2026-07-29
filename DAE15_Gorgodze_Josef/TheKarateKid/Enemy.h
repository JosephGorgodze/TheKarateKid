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

	void EnemyUpdate(float elapsedSec, const Rectf& playerBounds);
	void Draw() const;
	bool isAttacking() const;
	bool GetHasHit() const;
	void SetHasHit(bool hasHit);
	Rectf GetBounds() const;

private:
	Texture* m_pSpriteSheet{ nullptr };
	Rectf m_Bounds{ 500.f, 100.f, 80.f, 96.f };
	State m_State{ State::Idle };
	int m_FrameNr{};
	float m_AccuSec{};
	float m_FrameTime{ 0.15f };
	bool m_FacingRight{ false };
	float m_MoveSpeed{ 50.f };
	float m_MoveDirection{ -1.f };
	bool m_IsAttacking{ false };
	float m_TimeUntilAttack{};
	bool m_HasHit{};

	Rectf GetCurrentFrame() const;
	void UpdateAnimation(float elapsedSec);
	int GetFrameCount() const;
	bool AnimationLoops() const;

	void Think(const Rectf& playerBounds);
	void Move(float elapsedSec);
	void StartAttack();
	State previousState{ m_State };
};

