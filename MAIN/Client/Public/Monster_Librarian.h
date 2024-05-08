#pragma once

#include "Monster.h"

BEGIN(Engine)
class CPartObject;
class CEffect;
END

BEGIN(Client)

class CMonster_Librarian final : public CMonster
{
public:
	enum ANIMATION {
		ANIM_NONE = 0, ANIM_DIE, ANIM_FAST_SLAM, ANIM_FLINCH, ANIM_FLYING_DODGE, 
		ANIM_FLYING_IDLE = 5, ANIM_FLYING_IDLE_LEFT, ANIM_FLYING_IDLE_RIGHT, ANIM_SUMMON_ORB_END, ANIM_SUMMON_ORB_LOOP, 
		ANIM_SUMMON_ORB_START = 10, ANIM_SUMMON_ORB_END2, ANIM_FLYING_SWING_OVERHEAD, ANIM_FLYING_SWING_OVERHEAD2, ANIM_FLYING_SWING_WIDE, 
		ANIM_FLYING_SWOOP = 15, ANIM_ENTRY, ANIM_LIGHTNING_LAND, ANIM_LIGHTNING_LOOP, ANIM_LIGHTNING_RELEASE,
		ANIM_LIGHTNING_WAIT = 20, ANIM_LIGHTNING_WINDUP, ANIM_LIGHTNING_LAND2, ANIM_MELEE_WIDE, ANIM_MELEE_AOE, 
		ANIM_MELEE_THRUST = 25, ANIM_PALPATINE_EXIT, ANIM_PALPATINE_LOOP, ANIM_PALPATINE_EXIT2, ANIM_PALPATINE_LOOP2, 
		ANIM_PALPATINE_START = 30, ANIM_RETREAT, ANIM_TUGGED, ANIM_TURN, ANIM_ADDITIVE_FLINCH, 
		ANIM_END
	};

	enum STATE {
		STATE_ENTRY, STATE_IDLE, STATE_MELEE, STATE_DAMAGE,
		STATE_PATTERN_ENERGY_WAVE, STATE_PATTERN_ENERGY_BEAM, STATE_PATTERN_HOMING_ORBS, STATE_PATTERN_LIGHTNING_WARP, STATE_PATTERN_LUNGE_SWIPE, STATE_PATTERN_SUMMON_BAT,
		STATE_DIE, STATE_END
	};

	enum WEAPON {
		WEAPON_SWORD, WEAPON_END
	};

private:
	CMonster_Librarian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Librarian(const CMonster_Librarian& rhs);
	virtual ~CMonster_Librarian() = default;

public:
	// Set
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }
	void			Set_Damage_AccCoolTime(_float fCoolTime) { m_fAccDamageCoolTime = fCoolTime; }
	void			Set_isGroggy(_bool isGroggy) { m_isGroggy = isGroggy; }

	// Get
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }
	_float4			Get_Bone_Position(_uint iBoneIndex);
	_uint			Get_Pattern();
	//_bool			Get_isCollision();

	// State
	void			Change_State(STATE eState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool								m_isGroggy = { false };

	map<const wstring, CPartObject*>	m_PartObjects;
	queue<STATE>						m_States;

private:
	class CUI_BossHP*					m_pUIBossHP;

private:
	HRESULT			Add_Components();
	HRESULT			Add_PartObjects();
	HRESULT			Add_Patterns();
	HRESULT			Bind_ShaderResources();

	virtual HRESULT	Add_States();
	virtual void	Update_State();
	virtual void	Set_Animation();

public:
	static CMonster_Librarian* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
	void Damage_Event() override;
};

END