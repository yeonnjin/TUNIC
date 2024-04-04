#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonster_Spinner final : public CMonster
{
public:
	enum ANIMATION {
		ANIM_EXPLODE, ANIM_EMERGE, ANIM_FORWARD, ANIM_HIDDEN, ANIM_IDLE, 
		ANIM_RECOIL, ANIM_START_SPIN, ANIM_END
	};

	enum STATE { 
		STATE_IDLE, STATE_HIDDEN, STATE_EXPLODE, STATE_ATTACK, STATE_DAMAGE, 
		STATE_END 
	};

private:
	CMonster_Spinner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Spinner(const CMonster_Spinner& rhs);
	virtual ~CMonster_Spinner() = default;

public:
	// Set
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }

	// Get
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }
	_bool			Get_isCollision();
	
	// State
	void			Change_State(STATE eState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

	virtual HRESULT	Add_States();
	virtual void	Update_State();
	virtual void	Set_Animation();

public:
	static CMonster_Spinner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END