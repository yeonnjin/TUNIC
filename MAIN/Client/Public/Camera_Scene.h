#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CCamera_Scene final : public CCamera
{
private:
	CCamera_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Scene(const CCamera_Scene& rhs);
	virtual ~CCamera_Scene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Set_Level(_uint iLevel);

	virtual void	OnEnter(void* pArg);
	virtual void	OnExit();

private:
	_bool			m_isFinish = { false };

	_float			m_fAccEasingTime = { 0.f };
	_float			m_fEasingTime = { 2.f };

	_vector			m_vEnterPosition = {};
	_vector			m_vTargetPosition = {};

private:
	class CEasing*	m_pEasing;

public:
	static CCamera_Scene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

