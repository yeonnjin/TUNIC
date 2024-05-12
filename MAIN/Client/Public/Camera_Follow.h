#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CCamera_Follow final : public CCamera
{
public:
	enum STATE { START, UPDATE, _END, NONE, STATE_END };

public:
	typedef struct Camera_Follow_Desc : public CCamera::CAMERA_DESC
	{
		_bool	isLockOn;
		_vector vLookAt;
	}CAMERA_FOLLOW_DESC;

private:
	CCamera_Follow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Follow(const CCamera_Follow& rhs);
	virtual ~CCamera_Follow() = default;

public:
	void				Set_EnterBoss();
	void				Set_EnterShop();
	void				Set_Reset();

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void		Set_Level(_uint iLevel);

	virtual void		OnEnter(void* pArg);
	virtual void		OnExit();

private:
	_vector				m_vCamDistance = {};
	_float				m_fMoveTime = { 0.f };
	_float				m_fIdleTime = { 0.f };
	_float				m_fLookTime = { 0.f };

	_vector				m_vLookAt = {};

	_bool				m_isLockOn = { false };

	//_uint				m_iLevel = { LEVEL_END };

private:
	class CEasing*		m_pEasing;
	class CTransform*	m_pTargetTransform;
	class CPlayer*		m_pPlayer;

public:
	static CCamera_Follow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END