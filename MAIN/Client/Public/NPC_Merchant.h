#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CAnimator;
class CCollider;
class CTransform;
END

/* 蓖咯款 咯快 模备 */

BEGIN(Client)

class CNPC_Merchant final : public CGameObject
{
public:
	enum ANIMATION {
		ANIM_BLINK, ANIM_HIDDEN, ANIM_IDLE, ANIM_THANKYOU, ANIM_WAKEUP,
		ANIM_END
	};

	enum STATE {
		STATE_IDLE, STATE_HIDDEN, STATE_THANKYOU,
		STATE_END
	};

private:
	CNPC_Merchant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Merchant(const CNPC_Merchant& rhs);
	virtual ~CNPC_Merchant() = default;

public:
	// Set
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	
	// Get
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }
	_uint			Get_Current_Frame(_uint eAnimationIndex) { return m_pModelCom->Get_Current_Frame(eAnimationIndex); }

	// State
	void			Change_State(STATE eState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isBlend = { false };
	_bool			m_isMove = { false };

	_uint			m_eAnimationIndex = { 0 };
	_uint			m_eBlendAnimIndex = { 0 };
	_uint			m_eState = { 0 };

private:
	wstring			m_strModelComTag = {};
	CAnimator*		m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

private:
	void			Update_State();

private:
	HRESULT			Add_Components();
	HRESULT			Add_States();
	HRESULT			Bind_ShaderResources();
	void			Set_Animation();

public:
	static CNPC_Merchant* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END