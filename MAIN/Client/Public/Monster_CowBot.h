#pragma once

#include "Monster.h"

BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CMonster_CowBot final : public CMonster
{
public:
	enum ANIMATION {
		ANIM_ATTACK, ANIM_DIE, ANIM_PARRY, ANIM_X_QUICKTURN, ANIM_RUN,
		ANIM_BACKSWIPE, ANIM_HIT, ANIM_IDLE, ANIM_RECOIL, ANIM_X_RECOVER,
		ANIM_WALK, ANIM_END
	};

	enum STATE {
		STATE_IDLE, STATE_WALK, STATE_RUN, STATE_ATTACK, STATE_DAMAGE,
		STATE_PARRY, STATE_DIE, STATE_END
	};

	enum WEAPON {
		WEAPON_SWORD, WEAPON_SHIELD, WEAPON_END
	};

private:
	CMonster_CowBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_CowBot(const CMonster_CowBot& rhs);
	virtual ~CMonster_CowBot() = default;

public:
	// Set
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }
	//void			Set_isParry(_bool isParry) { m_isParry = isParry; }

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
	//_bool			m_isParry = { false };

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
	static CMonster_CowBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
	void Damage_Event() override;
};

END