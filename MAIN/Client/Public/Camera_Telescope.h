#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CCamera_Telescope final : public CCamera
{
private:
	CCamera_Telescope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Telescope(const CCamera_Telescope& rhs);
	virtual ~CCamera_Telescope() = default;

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
	_bool			m_isMove = { true };
	_bool			m_isStop = { false };

	_float			m_fAccMoveTime = { 0.f };
	_float			m_fMoveTime = { 1.f };

	_float			m_fAccStopTime = { 0.f };
	_float			m_fStopTime = { 2.f };

	_float			m_fAccOriginTime = { 0.f };
	_float			m_fOriginTime = { 1.f };

	_float			m_fCamFovy = {};
	_float			m_fTargetFovy = { XMConvertToRadians(65.f) };

	_vector			m_vCamPosition = {};
	_vector			m_vCamLook = {};
	_vector			m_vTargetPosition = { -19., 24.f, -132.5f, 1.f }; /*{ -5.3f, 28.7f, -134.5f, 1.f };*/
	_vector			m_vTargetLook = {};

private:
	class CEasing* m_pEasing = { nullptr };

private:

public:
	static CCamera_Telescope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END