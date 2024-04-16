#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CLibrarian_Effect_Orb final : public CEffect
{
public:
	typedef struct Librarian_Effect_Orb_Desc : public CEffect::EFFECT_DESC
	{
	}LIBRARIAN_EFFECT_ORB_DESC;

private:
	CLibrarian_Effect_Orb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLibrarian_Effect_Orb(const CLibrarian_Effect_Orb& rhs);
	virtual ~CLibrarian_Effect_Orb() = default;

public:
	void	Set_Pooling_Active() { m_isActive = true; }
	void	Set_Pooling_Inactive() { m_isInactive = true; }
	void	Set_Start_Position(_vector vPosition);
	void	Set_Target_Dir(_vector vDir) { m_pTransformCom->Look_At_Dir(vDir); }
	void	Set_Move(_bool isMove) { m_isMove = isMove; }
	void	Set_Index(_uint iIndex) {
		m_iIndex = iIndex;
		m_fStayTime = 0.2f * m_iIndex;
		m_fInactiveTime = 3.f + 0.2f * m_iIndex;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isActive = { false };
	_bool			m_isInactive = { false };
	_bool			m_isMove = { false };

	_uint			m_iIndex = { 0 };

	_float			m_fAccStayTime = { 0.f };
	_float			m_fStayTime = { 1.f };

	_float			m_fAccInactiveTime = { 0.f };
	_float			m_fInactiveTime = { 1.f };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLibrarian_Effect_Orb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CEffect을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END