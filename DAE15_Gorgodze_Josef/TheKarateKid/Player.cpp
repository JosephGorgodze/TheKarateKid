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
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	HandleMovement(elapsedSec, pStates);
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


void Player::HandleMovement(float elapsedSec, const Uint8* keyboardState)
{
	const float speed{ 150.f };
	State previousState{ m_State };
	m_State = State::Idle; // Default State

	if (keyboardState[SDL_SCANCODE_LEFT])
	{
		m_Bounds.left -= speed * elapsedSec;
		m_State = State::Walk;
		m_FacingRight = false;
	}
	else if (keyboardState[SDL_SCANCODE_RIGHT])
	{
		m_Bounds.left += speed * elapsedSec;
		m_State = State::Walk;
		m_FacingRight = true;
	}
	if (previousState != m_State)
	{
		m_FrameNr = 0;
		m_AccuSec = 0.f;
	}

	if (m_State == State::Walk)
	{
		m_AccuSec += elapsedSec;

		if (m_AccuSec >= m_FrameTime)
		{
			++m_FrameNr;

			if (m_FrameNr > 3)
			{
				m_FrameNr = 0;
			}
			m_AccuSec = 0.f;

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

	//return Rectf{ frameWidth * m_FrameNr, m_pSpriteSheet->GetHeight() - (row + 1) * frameHeight, frameWidth, frameHeight};
