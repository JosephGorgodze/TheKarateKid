#include "pch.h"
#include "Player.h"
#include <iostream>
#include <utils.h>
#include <cmath>

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
	m_HitsThisAttack = 0;
	m_LastHitFrame = -1;
}

void Player::ResetHealth()
{
	m_Health = GetMaxHealth();
}

void Player::PlayerUpdate(float elapsedSec, const std::vector<Vector2f>& groundVertices)
{
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	HandleMovement(elapsedSec, keyboardState, groundVertices);
	UpdateAnimation(elapsedSec);
	UpdateFall(elapsedSec);
	m_WasAttacking = AttackIsActive();
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
					m_State == State::CrouchKick ||
					m_State == State::CraneKick ||
					m_State == State::DrumPunch)
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


void Player::SetPosition(float x, float y)
{
	m_Bounds.left = x;
	m_Bounds.bottom = y;
}

void Player::SetPlatformerMode(bool platformer)
{
	m_PlatformerMode = platformer;
}

void Player::HandleGroundCollision(const std::vector<Vector2f>& groundVertices)
{
	if (m_VelocityY > 0.f)
	{
		return;
	}
	m_OnGround = false;
	Vector2f rayCastLeft{ m_Bounds.left, m_Bounds.bottom + 5.f };
	Vector2f rayEndLeft{ m_Bounds.left, m_Bounds.bottom - 10.f };
	Vector2f rayStartRight{ m_Bounds.left + m_Bounds.width, m_Bounds.bottom + 5.f};
	Vector2f rayEndRight{ m_Bounds.left + m_Bounds.width, m_Bounds.bottom - 10.f };

	utils::HitInfo hitInfoLeft{};
	utils::HitInfo hitInfoRight{};

	bool hitLeft = utils::Raycast(groundVertices, rayCastLeft, rayEndLeft, hitInfoLeft);
	bool hitRight = utils::Raycast(groundVertices, rayStartRight, rayEndRight, hitInfoRight);

	if (hitLeft && hitInfoLeft.normal.y > 0.5f)
	{
		m_Bounds.bottom = hitInfoLeft.intersectPoint.y;
		m_VelocityY = 0.f;
		m_OnGround = true;
	}
	else if (hitRight && hitInfoRight.normal.y > 0.5f)
	{
		m_Bounds.bottom = hitInfoRight.intersectPoint.y;
		m_VelocityY = 0.f;
		m_OnGround = true;
	}
}

bool Player::HandleWallCollision(const std::vector<Vector2f>& groundVertices, float movement)
{
	if (movement == 0.f)
	{
		return false;
	}

	Vector2f rayStart{};
	Vector2f rayEnd{};

	if (movement > 0.f)
	{
		rayStart = Vector2f{ m_Bounds.left + m_Bounds.width, m_Bounds.bottom + 5.f };
		rayEnd = Vector2f{ m_Bounds.left + m_Bounds.width + movement, m_Bounds.bottom + 5.f };
	}
	else
	{
		rayStart = Vector2f{ m_Bounds.left, m_Bounds.bottom + 5.f };
		rayEnd = Vector2f{ m_Bounds.left + movement, m_Bounds.bottom + 5.f };
	}

	utils::HitInfo hitInfo{};
	bool hit = utils::Raycast(groundVertices, rayStart, rayEnd, hitInfo);

	if (hit && std::abs(hitInfo.normal.x) > 0.5f)
	{
		return true;
	}

	return false;
}

void Player::HandleMovement(float elapsedSec, const Uint8* keyboardState, const std::vector<Vector2f>& groundVertices)
{
	State previousState{ m_State };
	if (!m_PlatformerMode)
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
		float movement = -m_MoveSpeed * elapsedSec;
		if (!HandleWallCollision(groundVertices, movement))
		{
			m_Bounds.left += movement;
		}
		if (m_OnGround)
		{
			m_State = State::Walk;
		}
		m_FacingRight = false;
	}
	else if (!m_IsCrouching && keyboardState[SDL_SCANCODE_RIGHT])
	{
		float movement = m_MoveSpeed * elapsedSec;
		if (!HandleWallCollision(groundVertices, movement))
		{
			m_Bounds.left += movement;
		}
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
		else if (m_State == State::Idle && m_CraneKickCount > 0)
		{
			UseCraneKick();
			StartAttack(State::CraneKick);
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
		else if (m_State == State::Idle && m_DrumPunchCount > 0)
		{
			UseDrumPunch();
			StartAttack(State::DrumPunch);
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

		if (!m_PlatformerMode && m_Bounds.bottom <= 100.f)
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
	case State::CraneKick:
		row = 10;
		break;
	case State::DrumPunch:
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
	case State::CraneKick:
		return 5;
	case State::DrumPunch:
		return 5;
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
	m_HitsThisAttack = 0;
	m_LastHitFrame = -1;
	m_PlayPunchSound = true;
}

void Player::StartFall(bool hitFromRight)
{
	++m_ComboHits;
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

bool Player::GetIsFalling() const
{
	return m_IsFalling;
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
		m_ComboHits = 0;
	}
}

int Player::GetCraneKickCount() const
{
	return m_CraneKickCount;
}

void Player::UseCraneKick()
{
	if (m_CraneKickCount > 0)
	{
		--m_CraneKickCount;
	}
}

int Player::GetDrumPunchCount() const
{
	return m_DrumPunchCount;
}

void Player::UseDrumPunch()
{
	if (m_DrumPunchCount > 0)
	{
		--m_DrumPunchCount;
	}
}

int Player::GetCurrentFrameNumber() const
{
	return m_FrameNr;
}

bool Player::CanHitFrame() const
{
	if (m_FrameNr == m_LastHitFrame)
		return false;
	return true;
}

void Player::RegisterHitFrame()
{
	m_LastHitFrame = m_FrameNr;
}

bool Player::GetFacingRight() const
{
	return m_FacingRight;
}

bool Player::GetIsAttacking() const
{
	return m_IsAttacking;
}

bool Player::JustStartedAttack()
{
	if (m_PlayPunchSound)
	{
		m_PlayPunchSound = false;
		return true;
	}
	return false;
}

bool Player::AttackIsActive() const
{
	if (!m_IsAttacking)
		return false;
	if (m_State == State::CraneKick)
	{
		return m_FrameNr == 2 || m_FrameNr == 3;
	}

	if (m_State == State::DrumPunch)
	{
		return m_FrameNr == 2 || m_FrameNr == 4;
	}
	return m_FrameNr == 1;
}

int Player::GetHitsThisAttack() const
{
	return m_HitsThisAttack;
}

void Player::AddHitThisAttack()
{
	++m_HitsThisAttack;
}

int Player::GetMaxHitsThisAttack()
{
	if (m_State == State::CraneKick)
		return 2;
	if (m_State == State::DrumPunch)
		return 2;

	return 1;
}

int Player::GetFrameNr() const
{
	return m_FrameNr;
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

Player::State Player::GetState() const
{
	return m_State;
}


void Player::ResetComboHits()
{
	m_ComboHits = 0;
}

int Player::GetComboHits() const
{
	return m_ComboHits;
}
int Player::GetMaxComboHits() const
{
	return m_MaxComboHits;
}