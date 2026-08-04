#include "pch.h"
#include "Player.h"
#include <iostream>

Player::Player()
{
	m_pSpriteSheet = new Texture("Daniel.png");

	std::cout << m_pSpriteSheet->GetWidth() << '\n';
	std::cout << m_pSpriteSheet->GetHeight() << '\n';
}

Player::~Player()
{
	delete m_pSpriteSheet;
	m_pSpriteSheet = nullptr;
}

void Player::Reset()
{
	m_Bounds.left = 100.f;
	m_Bounds.bottom = m_GroundY;
	m_State = State::Idle;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
	m_IsAttacking = false;
	m_IsFalling = false;
	m_IsCrouching = false;
	m_OnGround = true;
	m_VelocityY = 0.f;
	m_FallVelocityX = 0.f;
	m_FacingRight = true;
	m_HasHit = false;
}

void Player::ResetHealth()
{
	m_Health = GetMaxHealth();
}

void Player::PlayerUpdate(float elapsedSec)
{
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	HandleMovement(elapsedSec, keyboardState);
	UpdateAnimation(elapsedSec);
	UpdateFall(elapsedSec);
}

void Player::Draw() const
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

void Player::UpdateAnimation(float elapsedSec)
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
				m_FrameNr = 0;
				if (m_State == State::Punch ||
					m_State == State::Kick ||
					m_State == State::CrouchPunch ||
					m_State == State::CrouchKick)
				{
					m_IsAttacking = false;

					if (m_IsCrouching)
					{
						m_State = State::Crouch;
					}
					else
					{
						m_State = State::Idle;
					}
				}
			}
		}
		m_AccuSec = 0.f;
	}
}

bool Player::AnimationLoops() const
{
	switch (m_State)
	{
	case State::Walk:
			return true;
		default:
			return false;
	}
}


void Player::HandleMovement(float elapsedSec, const Uint8* keyboardState)
{
	State previousState{ m_State };
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

	if (m_IsAttacking || m_IsFalling)
	{
		return;
	}

	m_IsCrouching = false;
	
	if (m_OnGround)
	{
		m_State = State::Idle;
	}
	else
	{
		m_State = State::Jump;
	}

	if (keyboardState[SDL_SCANCODE_DOWN] && m_OnGround)
	{
		m_IsCrouching = true;
		m_State = State::Crouch;
	}

	if (!m_IsCrouching && keyboardState[SDL_SCANCODE_LEFT])
	{
		m_Bounds.left -= m_MoveSpeed * elapsedSec;
		if (m_OnGround)
		{
			m_State = State::Walk;
		}
		m_FacingRight = false;
	}
	else if (!m_IsCrouching && keyboardState[SDL_SCANCODE_RIGHT])
	{
		m_Bounds.left += m_MoveSpeed * elapsedSec;
		if (m_OnGround)
		{
			m_State = State::Walk;
		}
		m_FacingRight = true;
	}

	if (keyboardState[SDL_SCANCODE_UP] && m_OnGround && !m_IsCrouching)
	{
		m_VelocityY = 450.f;
		m_OnGround = false;
		m_State = State::Jump;
	}

	if (keyboardState[SDL_SCANCODE_Z] && !m_IsAttacking)
	{
		if (m_IsCrouching)
		{
			StartAttack(State::CrouchKick);
		}
		else
		{
			StartAttack(State::Kick);
		}
	}
	if (keyboardState[SDL_SCANCODE_X] && !m_IsAttacking)
	{
		if (m_IsCrouching)
		{
			StartAttack(State::CrouchPunch);
		}
		else
		{
			StartAttack(State::Punch);
		}
	}

	if (previousState != m_State)
	{
		m_FrameNr = 0;
		m_AccuSec = 0.f;
	}

	if (!m_OnGround && !m_IsFalling)
	{
		m_VelocityY += m_Gravity * elapsedSec;
		m_Bounds.bottom += m_VelocityY * elapsedSec;

		if (m_Bounds.bottom <= 100.f)
		{
			m_Bounds.bottom = 100.f;
			m_VelocityY = 0.f;
			m_OnGround = true;
		}
	}
	
}

Rectf Player::GetCurrentFrame() const
{
	float frameWidth{ 40.f };
	float frameHeight{ 48.f };
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

int Player::GetFrameCount() const
{
	switch (m_State)
	{
	case State::Idle:
		return 1;
	case State::Walk:
		return 4;
	case State::Jump:
		return 1;
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

void Player::StartAttack(State attackState)
{
	m_IsAttacking = true;
	m_State = attackState;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
	m_HasHit = false;
}

void Player::StartFall(bool hitFromRight)
{
	m_IsAttacking = false;
	m_OnGround = false;
	m_IsFalling = true;
	m_State = State::Fall;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
	m_VelocityY = 350.f;

	if (hitFromRight)
	{
		m_FallVelocityX = -220.f;
	}
	else
		m_FallVelocityX = 220.f;
}

void Player::Defeat()
{
	m_State = State::Hit;
	m_IsAttacking = false;
	m_IsFalling = false;
	m_OnGround = true;
	m_VelocityY = 0.f;
	m_FallVelocityX = 0.f;
	m_Bounds.bottom = m_GroundY;
	m_FrameNr = 0;
	m_AccuSec = 0.f;
}

int Player::GetHealth() const
{
	return m_Health;
}

int Player::GetMaxHealth() const
{
	return m_MaxHealth;
}

void Player::TakeDamage(int damage)
{
	m_Health -= damage;

	if (m_Health < 0)
	{
		m_Health = 0;
	}
}

void Player::UpdateFall(float elapsedSec)
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
		m_OnGround = true;
		m_FallVelocityX = 0.f;
		m_VelocityY = 0.f;
		m_State = State::Idle;
	}
}

void Player::SetHasHit(bool hasHit)
{
	m_HasHit = hasHit;
}

bool Player::GetHasHit() const
{
	return m_HasHit;
}

bool Player::GetFacingRight() const
{
	return m_FacingRight;
}

bool Player::GetIsAttacking() const
{
	return m_IsAttacking;
}

bool Player::AttackIsActive() const
{
	if (!m_IsAttacking)
		return false;
	return m_FrameNr == 1;
}

Rectf Player::GetBounds() const
{
	return m_Bounds;
}

Rectf Player::GetHurtBox() const
{
	if (m_State == State::Crouch || m_State == State::CrouchPunch || m_State == State::CrouchKick)
	{
		return Rectf{ m_Bounds.left + 20.f, m_Bounds.bottom, 40.f, 45.f };
	}
	return Rectf{ m_Bounds.left + 20.f, m_Bounds.bottom, 40.f, 90.f };
}

Rectf Player::GetAttackBox() const
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
		return Rectf{ m_Bounds.left + m_Bounds.width - 30.f, y + 30.f, 35.f, 25.f };
	}
	return Rectf{ m_Bounds.left, y + 30.f, 35.f, 25.f };
}
