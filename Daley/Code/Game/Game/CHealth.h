// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>



struct EntityDebugContext;
struct HitPayload;
struct TagQuery;



//----------------------------------------------------------------------------------------------------------------------
enum class HealthFlags : uint8_t
{
    // Statuses
	RegenSuppressed              = 1 << 0,
    CannotBeProtected            = 1 << 1,

    // State
	HealthReachedZero            = 1 << 2,

    // Single Frame Only
    IsProtectedThisFrame         = 1 << 3,
    IsSuppressingBurnThisFrame   = 1 << 4,
    IsSuppressingPoisonThisFrame = 1 << 5,
};



//----------------------------------------------------------------------------------------------------------------------
struct CHealth
{
public:

    CHealth() = default;
    explicit CHealth(void const* xmlElement);

    void TakeDamage(float damage);
    void TakeHealing(float healing);
    void TakePayload(HitPayload const& payload);

	float GetHealthFraction() const;
	float GetTotalBurnDamageRemaining() const;
    float GetBurnSaturation() const;
	float GetPoisonSaturation() const;
	float GetVulnerabilitySaturation() const;

    bool MatchesTagQuery(TagQuery const& query) const;

    bool GetIsTargetable() const;
    bool GetIsImmune() const;
    bool GetRegenSuppressed() const;
    bool GetCannotBeProtected() const;
    bool GetHealthReachedZero() const;
	bool GetIsProtected() const;
	bool GetIsSuppressingBurn() const;
	bool GetIsSuppressingPoison() const;

    void SetImmunityDuration(float duration);
	void SetRegenSuppressed(bool regenSuppressed); // maybe replace with a int counter for multiple sources of suppress regen
	void SetCannotBeProtected(bool cannotBeProtected);
	void SetHealthReachedZero(bool reachedZero);
	void SetIsProtectedThisFrame(bool isProtected);
	void SetIsSuppressingBurnThisFrame(bool isSuppressingBurn);
	void SetIsSuppressingPoisonThisFrame(bool isSuppressingPoison);

	void AppendDebugString(EntityDebugContext& context) const;

public:
    
	float m_maxHealth               = 100.f;
    float m_healthRegen             = 0.f;

	float m_currentHealth           = 100.f;
    float m_currentBurn             = 0.f;
    float m_currentPoison           = 0.f;
    float m_currentVulnerability    = 0.f;
	float m_damageTakenMultiplier   = 1.f; // Calculated as 1 + (vulnerability^X / 100) where X is vuln exponent
    float m_immunityDurationLeft    = 0.f; // Remaining immunity
   
	float m_physicalResistance      = 0.f;
	float m_burnResistance          = 0.f;
	float m_poisonResistance        = 0.f;

    uint8_t m_healthFlags           = 0;
};
