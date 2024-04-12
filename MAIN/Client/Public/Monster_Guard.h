#pragma once

#include "Monster.h"

BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CMonster_Guard final : public CMonster
{
public:
	enum ANIMATION {
		ANIM_BLOCK = 0, ANIM_CHASE, ANIM_DIE, ANIM_HURT, ANIM_TURN_L, 
		ANIM_TURN_R = 5, ANIM_AGGRO, ANIM_IDLE, ANIM_STUN
		, ANIM_END
	}; // parrystun

	enum STATE {
		STATE_IDLE, STATE_CHASE, STATE_BLOCK, STATE_DAMAGE,
		STATE_AGGRO, STATE_DIE, STATE_END
	};

	enum WEAPON {
		WEAPON_SHIELD, WEAPON_SPEAR, WEAPON_END
	};

private:
	CMonster_Guard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Guard(const CMonster_Guard& rhs);
	virtual ~CMonster_Guard() = default;

public:
	// Set
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }

	// Get
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }

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

private:
	HRESULT			Add_Components();
	HRESULT			Add_PartObjects();
	HRESULT			Bind_ShaderResources();

	virtual HRESULT	Add_States();
	virtual void	Update_State();
	virtual void	Set_Animation();

public:
	static CMonster_Guard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
	void Damage_Event() override;
};

END