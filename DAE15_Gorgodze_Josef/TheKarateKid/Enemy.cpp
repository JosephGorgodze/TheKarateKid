#include "pch.h"
#include "Enemy.h"
#include <cmath>
#include <iostream>

Enemy::Enemy()
{
	m_pSpriteSheet = new Texture("Enemy.png");
}

Enemy::~Enemy()
{
	delete m_pSpriteSheet;
	m_pSpriteSheet = nullptr;
}

void Enemy::EnemyUpdate(float elapsedSec, const Rectf& playerBounds)
{
	UpdateFall(elapsedSec);
	Think(playerBounds);
	Move(elapsedSec);
	UpdateAnimation(elapsedSec);
	if (m_TimeUntilAttack > 0.f)
	{
		m_TimeUntilAttack -= elapsedSec;
	}
}

void Enemy::Draw() const
{
	Rectf srcRect{ GetCurrentFrame() };

	if (m_FacingRight)
	{
		m_pSpriteSheet->Draw(m_Bounds, srcRect);
	}
	else
	{
		glPushMatrix();
		glTranslatef(m_Bounds.left + m_Bounds.width, 0.f, 0.f);
		glScalef(-1.f, 1.f, 1.f);
		Rectf drawRect{ 0.f, m_Bounds.bottom, m_Bounds.width, m_Bounds.height };
		m_pSpriteSheet->Draw(drawRect, srcRect);
		glPopMatrix();
	}
}

int Enemy::GetFrameCount() const
{
	switch (m_State)
	{
	case State::Walk:
		return 4;
	case State::Punch:
		return 2;
	case State::Kick:
		return 2;
	case State::CrouchPunch:
		return 2;
	case State::CrouchKick:
		return 2;
		
	default:
		return 1;
	}
}

bool Enemy::AnimationLoops() const
{
	return m_State == State::Walk;
}

void Enemy::Think(const Rectf& playerBounds)
{
	if (m_IsAttacking || m_IsFalling)
	{
		return;
	}
	State previousState{ m_State };
	float playerCenter = playerBounds.left + playerBounds.width / 2.f;
	float enemyCenter = m_Bounds.left + m_Bounds.width / 2.f;
	float distance = playerCenter - enemyCenter;
	if (abs(distance) > 60.f) //Range start attacking
	{
		m_State = State::Walk;

		if (distance > 0.f)
		{
			m_MoveDirection = 1.f;
			m_FacingRight = true;
		}
		else
		{
			m_MoveDirection = -1.f;
			m_FacingRight = false;
		}
	}
	else
	{
		if (!m_IsAttacking && m_TimeUntilAttack <= 0.f)
		{
			StartAttack();
		}
	}

	if (previousState != m_State)
	{
		m_FrameNr = 0;
		m_AccuSec = 0.f;
	}
}

void Enemy::Move(float elapsedSec)
{
	const float leftLimit{ 100.f };
	const float rightLimit{ 600.f };

	if (m_Bounds.left < leftLimit)
	{
		m_Bounds.left = leftLimit;
	}
	else if (m_Bounds.left > rightLimit)
	{
		m_Bounds.left = rightLimit;
	}

	if (m_State != State::Walk)
	{
		return;
	}
	m_Bounds.left += m_MoveDirection * m_MoveSpeed * elapsedSec;
}

void Enemy::StartAttack()
{
	m_IsAttacking = true;
	
	switch (rand()%4)
	{
	case 0:
		m_State = State::Punch;
		break;
	case 1:
		m_State = State::Kick;
		break;
	case 2:
		m_State = State::CrouchPunch;
		break;
	case 3:
		m_State = State::CrouchKick;
		break;
	}

	m_FrameNr = 0;
	m_AccuSec = 0.f;
	m_HasHit = false;
	m_TimeUntilAttack = 0.8f + float(rand() % 8) / 10.f; // Attack Cooldown
}

bool Enemy::isAttacking() const
{
	return m_IsAttacking;
}

bool Enemy::GetHasHit() const
{
	return m_HasHit;
}

void Enemy::SetHasHit(bool hasHit)
{
	m_HasHit = hasHit;
}

bool Enemy::GetFacingRight() const
{
	return m_FacingRight;
}

int Enemy::GetComboHits() const
{
	return m_ComboHits;
}

int Enemy::GetMaxComboHits() const
{
	return m_MaxComboHits;
}

void Enemy::UpdateAnimation(float elapsedSec)
{
	if (GetFrameCount() == 1)
	{
		return;
	}

	m_AccuSec += elapsedSec;

	if (m_AccuSec >= m_FrameTime)
	{
		++m_FrameNr;

		if (m_FrameNr >= GetFrameCount())
		{
			if (AnimationLoops())
			{
				m_FrameNr = 0;
			}
			else
			{
				m_IsAttacking = false;
				m_State = State::Idle;
				m_FrameNr = 0;
			}
		}

		m_AccuSec = 0.f;
	}
}

Rectf Enemy::GetCurrentFrame() const
{
	const float frameWidth{ 40.f };
	const float frameHeight{ 48.f };
	int row{};

	switch (m_State)
	{
	case State::Idle:
		row = 0;
		break;

	case State::Walk:
		row = 1;
		break;
	case State::Jump:
		row = 2;
		break;
	case State::Crouch:
		row = 3;
		break;
	case State::Punch:
		row = 4;
		break;
	case State::Kick:
		row = 5;
		break;
	case State::CrouchPunch:
		row = 6;
		break;
	case State::CrouchKick:
		row = 7;
		break;
	case State::Hit:
		row = 9;
		break;
	case State::Fall:
		row = 8;
		break;
	default:
		row = 0;
		break;
	}
	return Rectf{ m_FrameNr * frameWidth, row * frameHeight, frameWidth, frameHeight };
}

Rectf Enemy::GetBounds() const
{
	return m_Bounds;
}

Rectf Enemy::GetAttackBox() const
{
	if (!m_IsAttacking)
	{
		return Rectf{};
	}
	
	float y{ m_Bounds.bottom + 25.f };

	switch (m_State)
	{
	case State::CrouchPunch:
	case State::CrouchKick:
		y = m_Bounds.bottom + 5.f;
		break;

	default:
		y = m_Bounds.bottom + 25.f;
		break;
	}

	if (m_FacingRight)
	{
		return Rectf{ m_Bounds.left + m_Bounds.width - 40.f, y + 30.f, 35.f, 25.f };
	}

	return Rectf{ m_Bounds.left, y + 30.f, 35.f, 25.f };
}

Rectf Enemy::GetHurtBox() const
{
	if (m_State == State::Crouch || m_State == State::CrouchPunch || m_State == State::CrouchKick)
	{
		return Rectf{ m_Bounds.left + 20.f, m_Bounds.bottom, 40.f, 45.f };
	}
	return Rectf{ m_Bounds.left + 20.f, m_Bounds.bottom, 40.f, 90.f };
}

bool Enemy::AttackIsActive() const
{
	if (!m_IsAttacking)
		return false;
	return m_FrameNr == 1;
}

void Enemy::StartFall(bool hitFromRight)
{
	++m_ComboHits;
	m_IsAttacking = false;
	m_IsFalling = true;
	m_State = State::Fall;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
	m_VelocityY = 350.f;

	if (m_ComboHits >= m_MaxComboHits)
	{
		return;
	}

	if (hitFromRight)
	{
		m_FallVelocityX = -220.f;
	}
	else
		m_FallVelocityX = 220.f;
}

void Enemy::UpdateFall(float elapsedSec)
{
	if (!m_IsFalling)
		return;
	m_VelocityY += m_Gravity * elapsedSec;
	m_Bounds.left += m_FallVelocityX * elapsedSec;
	m_Bounds.bottom += m_VelocityY * elapsedSec;

	if (m_Bounds.bottom <= m_GroundY)
	{
		m_Bounds.bottom = m_GroundY;
		m_IsFalling = false;
		m_FallVelocityX = 0.f;
		m_VelocityY = 0.f;
		m_ComboHits = 0;
		m_State = State::Idle;
	}
}

int Enemy::GetHealth() const
{
	return m_Health;
}

int Enemy::GetMaxHealth() const
{
	return m_MaxHealth;
}

void Enemy::TakeDamage(int damage)
{
	m_Health -= damage;

	if (m_Health < 0)
	{
		m_Health = 0;
	}
}

void Enemy::Reset()
{
	m_Bounds.left = 500.f;
	m_Bounds.bottom = m_GroundY;
	m_State = State::Idle;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
	m_IsAttacking = false;
	m_IsFalling = false;
	m_VelocityY = 0.f;
	m_FallVelocityX = 0.f;
	m_MoveDirection = -1.f;
	m_FacingRight = false;
	m_HasHit = false;
	m_TimeUntilAttack = 0.f;
	m_ComboHits = 0;
}

void Enemy::ResetHealth()
{
	m_Health = GetMaxHealth();
}

void Enemy::Defeat()
{
	m_State = State::Hit;
	m_IsAttacking = false;
	m_IsFalling = false;
	m_VelocityY = 0.f;
	m_FallVelocityX = 0.f;
	m_Bounds.bottom = m_GroundY;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
}
