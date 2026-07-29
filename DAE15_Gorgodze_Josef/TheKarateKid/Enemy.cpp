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
	if (m_IsAttacking)
	{
		return;
	}
	State previousState{ m_State };
	float playerCenter = playerBounds.left + playerBounds.width / 2.f;
	float enemyCenter = m_Bounds.left + m_Bounds.width / 2.f;
	float distance = playerCenter - enemyCenter;
	if (abs(distance) > 35.f) //Range start attacking
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
		row = 10;
		break;
	case State::Fall:
		row = 11;
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