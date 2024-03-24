#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Focus final : public CCamera
{
public:
	enum STATE { START, UPDATE, _END, NONE, STATE_END };

public:
	typedef struct Camera_Focus_Desc : public CCamera::CAMERA_DESC
	{
		class CTransform* pTargetTransform;
	}CAMERA_FOCUS_DESC;

private:
	CCamera_Focus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Focus(const CCamera_Focus& rhs);
	virtual ~CCamera_Focus() = default;

public:
	//void	Set_Target_Transform(class CTransform* pTagetTransform) { m_pTargetTransform = pTagetTransform; Safe_AddRef(m_pTargetTransform); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_vector				m_PrePosition = {};
	_vector				m_CurPosition = {};
	_vector				m_CamDistance = {};

	STATE				m_ePreState = { STATE_END };
	STATE				m_eState = { STATE_END };
	_float				m_fEaseTime[STATE_END] = {};
	_float				m_fCurTime = {};
	//_float				m_fSpeed = { 5.f };

private:
	class CTransform*	m_pTargetTransform;
	class CPlayer*		m_pPlayer;

private:
	_float			EaseInOutQuad(_float fStart, _float fEnd, _float fValue);
	_float			EaseInQuart(_float fStart, _float fEnd, _float fValue);
	_float			EaseOutQuart(_float fStart, _float fEnd, _float fValue);

public:
	static CCamera_Focus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END