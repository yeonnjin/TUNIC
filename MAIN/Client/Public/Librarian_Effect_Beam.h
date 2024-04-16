#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CLibrarian_Effect_Beam final : public CEffect
{
public:
	enum ANIMATION { ANIM_DEPLOY, ANIM_SUSTAIN, ANIM_OFF, ANIM_END };
public:
	typedef struct Librarian_Effect_Beam_Desc : public CEffect::EFFECT_DESC
	{
	}LIBRARIAN_EFFECT_BEAM_DESC;

private:
	CLibrarian_Effect_Beam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLibrarian_Effect_Beam(const CLibrarian_Effect_Beam& rhs);
	virtual ~CLibrarian_Effect_Beam() = default;

public:
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }

	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }
	//void			Set_Pooling_Active() { m_isActive = true; }
	void			Set_Pooling_Inactive() { m_isInactive = true; }

	void			Start_Pattern(_fvector vStartPosition, _fvector vDir);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isActive = { false };
	_bool			m_isInactive = { false };
	_bool			m_isBlend = { false };

	_uint			m_eAnimationIndex = { 0 };
	_uint			m_eBlendAnimIndex = { 0 };
	_uint			m_iPattern = { 0 };

	_float			m_fAccLiveTime = { 0.f };
	_float			m_fLiveTime = { 1.f };

private:
	CAnimator*		m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	HRESULT			Add_Components();
	HRESULT			Bind_ShaderResources();

	virtual void	Set_Animation();

	void			Play_Pattern(_float fTimeDelta);

public:
	static CLibrarian_Effect_Beam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CEffect을(를) 통해 상속됨
	 void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END