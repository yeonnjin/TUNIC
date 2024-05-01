#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CCamera_Top final : public CCamera
{
private:
	CCamera_Top(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Top(const CCamera_Top& rhs);
	virtual ~CCamera_Top() = default;

public:
	_bool	Get_isIdle() { if (false == m_isDown && false == m_isBack) return true; else return false; }

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
	_bool			m_isDown = { true };
	_bool			m_isBack = { false };

	_float			m_fDistance = { 40.f };

	_float			m_fAccDownTime = { 0.f };
	_float			m_fDownTime = { 2.f };

	_float			m_fAccBackTime = { 0.f };
	_float			m_fBackTime = { 2.5f };

	_vector			m_vDownPosition = {};
	_vector			m_vBackPosition = {};

private:
	class CEasing* m_pEasing;

public:
	static CCamera_Top* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END