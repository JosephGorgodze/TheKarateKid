#pragma once
#include <Texture.h>
#include <vector>
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
		Fall,
		CraneKick,
		DrumPunch
	};
	Player();
	~Player();
	void Reset();
	void ResetHealth();
	void PlayerUpdate(float elapsedSec, const std::vector<Vector2f>& groundVertices = {});
	void Draw() const;
	void UpdateAnimation(float elapsedSec);
	bool AnimationLoops() const;
	Rectf GetBounds() const;

	bool GetIsAttacking() const;
	bool JustStartedAttack();
	bool AttackIsActive() const;
	int GetHitsThisAttack() const;
	void AddHitThisAttack();
	int GetMaxHitsThisAttack();
	int GetFrameNr() const;
	bool GetFacingRight() const;

	int GetCraneKickCount() const;
	void UseCraneKick();
	int GetDrumPunchCount() const;
	void UseDrumPunch();

	int GetCurrentFrameNumber() const;
	bool CanHitFrame() const;
	void RegisterHitFrame();
	void StartFall(bool hitFromRight);
	void Defeat();
	bool GetIsFalling() const;
	Rectf GetHurtBox() const;
	Rectf GetAttackBox() const;
	State GetState() const;

	int GetComboHits() const;
	int GetMaxComboHits() const;
	void ResetComboHits();

	int GetHealth() const;
	int GetMaxHealth() const;
	void TakeDamage(int damage);

	//Platformer
	void SetPosition(float x, float y);
	void SetPlatformerMode(bool platformer);
	void HandleGroundCollision(const std::vector<Vector2f>& groundVertices);
	bool HandleWallCollision(const std::vector<Vector2f>& groundVertices, float movement);
private:
	void HandleMovement(float elapsedSec, const Uint8* keyboardState, const std::vector<Vector2f>& groundVertices); //Detects state of every key on the keyboard

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
	int m_HitsThisAttack{};
	bool m_WasAttacking{};
	bool m_PlayPunchSound{};
	int m_LastHitFrame{ -1 };

	int m_CraneKickCount{ 2 };
	int m_DrumPunchCount{ 2 };

	int m_ComboHits{};
	const int m_MaxComboHits{ 3 };

	int m_Health{20};
	static constexpr int m_MaxHealth{ 20 };

	//Knockback
	bool m_IsFalling{};
	float m_FallVelocityX{};

	Rectf GetCurrentFrame() const;
	int GetFrameCount() const;
	void StartAttack(State attackState);
	void UpdateFall(float elapsedSec);

	//Platformer
	bool m_PlatformerMode{};
};

