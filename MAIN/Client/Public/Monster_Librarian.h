#pragma once

#include "Monster.h"

BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CMonster_Librarian final : public CMonster
{
public:
	enum ANIMATION {
		ANIM_ADDITIVE_FLINCH, ANIM_DIE, ANIM_FAST_SLAM, ANIM_FLINCH, ANIM_FLYING_DODGE,
		ANIM_FLYING_IDLE, ANIM_FLYING_IDLE_LEFT, ANIM_FLYING_IDLE_RIGHT, ANIM_FLYING_HAND_DOWN, ANIM_FLYING_HAND_UP,
		ANIM_FLYING_SWING, ANIM_FLYING_SWOOP, ANIM_IDLE_GHOST, ANIM_ENTRY, ANIM_LIGHTNING, 
		ANIM_LIGHTNING_LOOP, ANIM_LIGHTNING_DRAW, ANIM_LIGHTNING_UP, ANIM_LIGHTNING_UP2, ANIM_LIGHTNING_SLAM,
		ANIM_MELEE_WIDE, ANIM_MELEE_WIDE2, ANIM_MELEE_THRUST, ANIM_PALPATINE, ANIM_PALPATINE_EXIT,
		ANIM_PALPATINE_ONCE, ANIM_PALPATINE2, ANIM_RETREAT, ANIM_TUGGED, ANIM_TURN, ANIM_END
	};

	enum STATE {
		STATE_ENTRY, STATE_IDLE, 
		STATE_PATTERN_ENERGY_WAVE, STATE_PATTERN_ENERGY_BEAM, STATE_PATTERN_HOMING_ORBS, STATE_PATTERN_LIGHTNING_WARP, STATE_PATTERN_LUNGE_SWIPE, 
		STATE_DIE, STATE_END
	};

	enum WEAPON {
		WEAPON_SWORD, WEAPON_SHIELD, WEAPON_END
	};

private:
	CMonster_Librarian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Librarian(const CMonster_Librarian& rhs);
	virtual ~CMonster_Librarian() = default;

public:
	// Set
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }

	// Get
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }
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
	map<const wstring, CPartObject*>	m_PartObjects;
	queue<STATE>	m_States;

private:
	HRESULT			Add_Components();
	HRESULT			Add_PartObjects();
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