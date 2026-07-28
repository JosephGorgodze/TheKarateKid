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

void Player::PlayerUpdate(float elapsedSec)
{
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	HandleMovement(elapsedSec, keyboardState);
	UpdateAnimation(elapsedSec);
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
	const float speed{ 150.f };
	State previousState{ m_State };

	if (m_IsAttacking)
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
		m_Bounds.left -= speed * elapsedSec;
		if (m_OnGround)
		{
			m_State = State::Walk;
		}
		m_FacingRight = false;
	}
	else if (!m_IsCrouching && keyboardState[SDL_SCANCODE_RIGHT])
	{
		m_Bounds.left += speed * elapsedSec;
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

	if (!m_OnGround)
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
}

Rectf Player::GetBounds() const
{
	return m_Bounds;
}

Rectf Player::GetHurtBox() const
{
	return Rectf{ m_Bounds.left + 20.f, m_Bounds.bottom, 40.f, 90.f };
}

Rectf Player::GetAttackBox() const
{
	return Rectf();
	if (!m_IsAttacking)
	{
		return Rectf{};
	}
	if (m_FacingRight)
	{
		return Rectf{ m_Bounds.left + m_Bounds.width, m_Bounds.bottom + 20.f, 35.f, 25.f };
	}
	return Rectf{ m_Bounds.left - 35.f, m_Bounds.bottom + 20.f, 35.f, 25.f };
}
